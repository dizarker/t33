#ifndef _DATALOG_H_
#define _DATALOG_H_

#include "include.h"

#define LOGGER_CMD_READSTATE				((uint8_t)0x01)
#define LOGGER_CMD_WRITE						((uint8_t)0x02)
#define LOGGER_CMD_READ							((uint8_t)0x03)
#define LOGGER_CMD_ERASE						((uint8_t)0x04)

#define LOGGER_SECTOR_SIZE					2100

typedef struct 
{
	uart_t *uart;
	uint8_t rxState;
	uint8_t buffer[2048];
	uint8_t *rxPnt;
	uint16_t lenToRcv;
} loggerRcv_t;

typedef struct
{
	uint8_t ready;
	uint32_t recNum;
	uint16_t recLength;
	uint8_t data[2048];
} record_t;

typedef struct
{
	loggerRcv_t receiver;
	uint8_t ready;
	uint32_t memTotal;
	uint32_t memUsed;
	uint32_t lastFileNum;
	record_t record;
} logger_t;

// Применяем директиву выравнивания
#pragma pack (push, 1)

typedef struct
{
	uint8_t stByte1;
	uint8_t stByte2;
	uint8_t stByte3;
	uint16_t len;
	uint16_t _len;
	uint8_t opcode;
	uint32_t fileNum;
	uint8_t data[];
} logPacket_t;

typedef struct
{
	uint8_t stByte1;
	uint8_t stByte2;
	uint8_t stByte3;
	uint16_t len;
	uint16_t _len;
	uint8_t opcode;
	uint32_t totalMemory;
	uint32_t usedMemory;
	uint32_t lastRecord;
	uint32_t crc32;
} logger_info_t;

#pragma pack (pop)
// Конец дириктивы выравнивания

void loggerInit (logger_t *logger, uart_t *uart);
void loggerRxTask (logger_t *logger);
void loggerGetData (logger_t *logger, uint32_t recNum);
void logger_sendData (logger_t *logger, uint32_t recNum, uint8_t *msg, uint16_t len);
void loggerReadMem (logger_t *logger);
void loggerEraseData (logger_t *logger);

#endif
