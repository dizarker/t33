#include "include.h"

static uint8_t readFlashData[] = {0xb0, 0x3b, 0x5e, 0x05, 0x00, 0xfa, 0xff, 0x01, 0x8f, 0x43, 0xb2, 0x52};
static uint8_t eraseFlashData[] = {0xb0, 0x3b, 0x5e, 0x05, 0x00, 0xfa, 0xff, 0x04, 0x00, 0xb7, 0xd8, 0x22};

static void loggerRxCallback (logger_t *logger, int ch);
static void loggerProcessAnswer (logger_t *logger);

void loggerInit (logger_t *logger, uart_t *uart)
{
	logger->receiver.uart = uart;
	logger->receiver.rxPnt = logger->receiver.buffer;
	logger->receiver.rxState = 0;
	logger->lastFileNum = 0;
	logger->memTotal = 0;
	logger->memUsed = 0;
	logger->ready = 0;
	logger->record.ready = 0;
	
	// Инитим логгер
	uart_init (logger->receiver.uart, 230400);
	
	// Запросим состояние памяти
	loggerReadMem (logger);
}

void logger_sendData (logger_t *logger, uint32_t recNum, uint8_t *msg, uint16_t len)
{
	uint32_t *crc;
	logPacket_t *log = (void*)logger->receiver.buffer;
	
	// Выставим константы
	log->stByte1 = 0xb0;
	log->stByte2 = 0x3b;
	log->stByte3 = 0x5e;
	log->len = sizeof (logPacket_t) - 3 + len; // 3 байта - префикс, который не участвует в длине
	log->_len = ((~log->len) & 0xffff);
	log->opcode = LOGGER_CMD_WRITE;
	log->fileNum = recNum;
	
	memcpy (log->data, msg, len);
	
	crc = (uint32_t*)((uint8_t*)log + sizeof (logPacket_t) + len);
	*crc = crc32hw ((void*)&log->len, sizeof (logPacket_t) + len - 3, 1); // -3 - это опять те самые 3 байта префикса, которые  не в длине, ни в подсчете ЦРЦ не участвуют
	
	uart_put_block (logger->receiver.uart, (void*)log, sizeof (logPacket_t) + len + sizeof (uint32_t));
}

void loggerRxTask (logger_t *logger)
{
	int n;
	for (;;)
	{
		n = uart_getc (logger->receiver.uart);
		if (n >= 0) loggerRxCallback (logger, n);
		else break;
	}
}

static void loggerRxCallback (logger_t *logger, int ch)
{
	uint8_t byte;
	uint32_t crcPending, *crcCurrent;
	
	byte = (uint8_t)ch;
	*logger->receiver.rxPnt = byte;
	
	// Парсим входные байтики
	switch (logger->receiver.rxState)
	{
		case 0:
			// Ждем первый старт байт
			if (byte == 0xb0)
			{	
				logger->receiver.rxPnt++;
				logger->receiver.rxState++;
			}
			break;
			
		case 1:
			// Второй старт байт 
			if (byte == 0x3b)
			{
				logger->receiver.rxPnt++;
				logger->receiver.rxState++;
			}
			else
			{
				logger->receiver.rxPnt = logger->receiver.buffer;
				logger->receiver.rxState = 0;
			}
			break;
			
		case 2:
			// Третий старт байт
			if (byte == 0x5e)
			{
				logger->receiver.rxPnt++;
				logger->receiver.rxState++;
			}
			else
			{
				logger->receiver.rxPnt = logger->receiver.buffer;
				logger->receiver.rxState = 0;
			}
			break;
			
		case 3:
			// Первый байт длины
			logger->receiver.lenToRcv = byte;
			logger->receiver.rxPnt++;
			logger->receiver.rxState++;
			break;
		
		case 4:
			// Второй байт длины
			logger->receiver.lenToRcv |= (byte << 8);
			logger->receiver.rxPnt++;
			logger->receiver.rxState++;
			break;
		
		case 5:
			// Первый байт инверсии длины
			if ((logger->receiver.lenToRcv & 0x00ff) == (~byte & 0x00ff))
			{
				logger->receiver.rxPnt++;
				logger->receiver.rxState++;
			}
			else
			{
				logger->receiver.rxPnt = logger->receiver.buffer;
				logger->receiver.rxState = 0;
			}
			break;
			
		case 6:
			// Второй байт инверсии длины
			if (((logger->receiver.lenToRcv & 0xff00) >> 8) == (~byte & 0x00ff))
			{
				logger->receiver.rxPnt++;
				logger->receiver.rxState++;
			}
			else
			{
				logger->receiver.rxPnt = logger->receiver.buffer;
				logger->receiver.rxState = 0;
			}
			break;
			
		case 7:
			// Принимаем данные
			logger->receiver.rxPnt++;
			logger->receiver.lenToRcv--;
			if (logger->receiver.lenToRcv == 0)
			{
				// Поле длины составляет 4 байта, но они уже приняты. С другой стороны црц32 тоже занимает 4 байта. Поэтому будем принимать, пока в не уйдем в 0.
				// Посчитаем crc
				crcCurrent = (uint32_t*)(logger->receiver.rxPnt - 4);		// Указатель на контрольную сумму. Это будет от указателя на следующий байт, который будет принят, минус длина crc32
				crcPending = crc32hw (logger->receiver.buffer + 3, logger->receiver.rxPnt - logger->receiver.buffer - 7, 1); // Считаем crc32. Префикс (первые 3 байта) не обсчитываем, из длины всего пакета не будут в обсчете 7 байт (префикс + црц32)
				if (crcPending == *crcCurrent) 
				{
					loggerProcessAnswer (logger);
				}
				else
				{
					__nop();
				}
				logger->receiver.rxState = 0;
				logger->receiver.rxPnt = logger->receiver.buffer;			
			}
			
		default: break;
	}	
}

void loggerReadMem (logger_t *logger)
{
	uart_put_block (logger->receiver.uart, readFlashData, sizeof (readFlashData));
}

static void loggerProcessAnswer (logger_t *logger)
{
	logPacket_t *log = (void*)logger->receiver.buffer;
	logger_info_t *logInfo;
	
	switch (log->opcode)
	{
		case LOGGER_CMD_READSTATE:
			// Данные о состоянии памяти
			logInfo = (void*)logger->receiver.buffer;
			logger->memTotal = logInfo->totalMemory;
			logger->memUsed = logInfo->usedMemory;
			logger->lastFileNum = logInfo->lastRecord;
			logger->ready = 1;
			break;
		
		case LOGGER_CMD_WRITE:
			// Приняли подтверждение того, что файл был записа
			logger->lastFileNum++;
			logger->memUsed += LOGGER_SECTOR_SIZE / 1000;
			break;
		
		case LOGGER_CMD_READ:
			// Приняли данные
			logger->record.ready = 1;
			logger->record.recLength = log->len - sizeof (logPacket_t) + 3;
			logger->record.recNum = log->fileNum;
			memcpy (logger->record.data, log->data, log->len - sizeof (logPacket_t) + 3);
			break;
		
		case LOGGER_CMD_ERASE:
			// Приняли подтверждение, что все стерто
			logger->lastFileNum = 0;
			logger->memUsed = 0;
			break;
		
		default: break;
	}
}

void loggerGetData (logger_t *logger, uint32_t recNum)
{
	uint32_t *crc;
	logPacket_t *log = (void*)logger->receiver.buffer;
	
	// Выставим константы
	log->stByte1 = 0xb0;
	log->stByte2 = 0x3b;
	log->stByte3 = 0x5e;
	log->len = sizeof (logPacket_t) - 3;
	log->_len = ((~log->len) & 0xffff);
	log->opcode = LOGGER_CMD_READ;
	log->fileNum = recNum - 1;
	
	crc = (uint32_t*)((uint8_t*)log + sizeof (logPacket_t));
	*crc = crc32hw ((void*)&log->len, sizeof (logPacket_t) - 3, 1);
	
	uart_put_block (logger->receiver.uart, (void*)log, sizeof (logPacket_t) + sizeof (uint32_t));	
}

void loggerEraseData (logger_t *logger)
{
	uart_put_block (logger->receiver.uart, (void*)eraseFlashData, sizeof (eraseFlashData));
}
