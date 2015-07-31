#include "nrfRadio.h"

void nrfInit (nrf24l01_t *iData)
{
	GPIO_InitTypeDef sGPIO;
	SPI_InitTypeDef sSPI;
	
	// Инициализируем лапки
	// SPI
	sGPIO.GPIO_Mode = GPIO_Mode_AF;
	sGPIO.GPIO_OType = GPIO_OType_PP;
	sGPIO.GPIO_Pin = (1 << SPI_SCK_Pin) | (1 << SPI_MOSI_Pin) | (1 << SPI_MISO_Pin);
	sGPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	sGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (SPI_PORT, &sGPIO);
	
	GPIO_PinAFConfig (SPI_PORT, SPI_SCK_Pin, GPIO_AF_SPI1);
	GPIO_PinAFConfig (SPI_PORT, SPI_MISO_Pin, GPIO_AF_SPI1);
	GPIO_PinAFConfig (SPI_PORT, SPI_MOSI_Pin, GPIO_AF_SPI1);
	
	// CS от SPI
	sGPIO.GPIO_Mode = GPIO_Mode_OUT;
	sGPIO.GPIO_Pin = (1 << SPI_CS_Pin);
	GPIO_Init (SPI_PORT, &sGPIO);
	
	// Chip enable
	sGPIO.GPIO_Pin = (1 << CHIPEN_Pin);
	GPIO_Init (CHIPEN_PORT, &sGPIO);
	
	// Interrupt
	sGPIO.GPIO_Mode = GPIO_Mode_IN;
	sGPIO.GPIO_Pin = (1 << INT_Pin);
	sGPIO.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init (INT_PORT, &sGPIO);
	
	SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	CHIPEN_PORT->ODR &=~
	(1 << CHIPEN_Pin);
	
	// Инициализируем SPI
	sSPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	sSPI.SPI_CPHA = SPI_CPHA_1Edge;
	sSPI.SPI_CPOL = SPI_CPOL_Low;
	sSPI.SPI_DataSize = SPI_DataSize_8b;
	sSPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	sSPI.SPI_FirstBit = SPI_FirstBit_MSB;
	sSPI.SPI_Mode = SPI_Mode_Master;
	sSPI.SPI_NSS = SPI_NSS_Soft;
	SPI_Init (SPIx, &sSPI);
	
	SPI_Cmd (SPIx, ENABLE);
	
	// А теперь начинаем конфигурировать
	nrfSetChannel (iData->channel);
		
	// Выставляем длину адреса
	nrfWriteReg (NRF_REG_AWIDTH, iData->AddressWidth);
	
	// Настройки повторов
	nrfWriteReg (NRF_REG_RETR, ((iData->RetransmissionDelay << 4) | (iData->RetransmissionCount)));	
	
	// Настройки канала и усилителя
	nrfWriteReg (NRF_REG_RFSETUP, (iData->DataRate << 3) | (iData->PALevel << 1) | 0x01);
	
	// Старт ап
	nrfWriteReg (NRF_REG_CONFIG, RF_CFG_REG_VAL);
	
	delay_us (10000);
	
	iData->pipeMask = 0x01;
}

uint8_t spiXmit (uint8_t byte)
{
	SPIx->DR = byte;
	while (!(SPIx->SR & SPI_SR_RXNE));	
	return SPIx->DR;
}

void delay_us (uint16_t delay)
{
	TIM4->PSC = 99;
	TIM4->ARR = delay;
	TIM4->EGR = TIM_EGR_UG;
	TIM4->CR1 = (TIM_CR1_CEN | TIM_CR1_OPM);
	while ((TIM4->CR1 & TIM_CR1_CEN) != 0);
}

void delay_ms (uint16_t delay)
{
	TIM4->PSC = 9999;
	TIM4->ARR = delay * 10;
	TIM4->EGR = TIM_EGR_UG;
	TIM4->CR1 = (TIM_CR1_CEN | TIM_CR1_OPM);
	while ((TIM4->CR1 & TIM_CR1_CEN) != 0);
}

void nrfWriteReg (uint8_t addr, uint8_t value)
{
	SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
	delay_us (1);
	spiXmit (addr | 0x20);
	spiXmit (value);	
	SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	delay_us (1);
}

void nrfWriteRegMulti (uint8_t addr, uint8_t *data, uint8_t len)
{
	uint8_t n;
	
	SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
	delay_us (1);
	spiXmit (addr | NRF_CMD_WRITEREG);
	for (n = 0; n < len; n++) spiXmit (*(data + n));	
	SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	delay_us (1);
}

uint8_t nrfReadReg (uint8_t addr)
{
	uint8_t n;
	
	SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
	delay_us (1);
	spiXmit (addr);
	n = spiXmit (0x00 | NRF_CMD_READREG);	
	SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	delay_us (1);
	
	return n;
}

void nrfReadRegMulti (uint8_t addr, uint8_t *data, uint8_t len)
{
	uint8_t n;
	
	SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
	delay_us (1);
	spiXmit (addr);
	for (n = 0; n < len; n++) *(data + n) = spiXmit (0x00);	
	SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	delay_us (1);
}

void nrfWriteData (uint8_t cmd, uint8_t *data, uint8_t len)
{
	uint8_t n;
	
	SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
	delay_us (1);
	spiXmit (cmd);
	for (n = 1; n <= len; n++) spiXmit (*(data + len - n));	
	SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	delay_us (1);
}

void nrfReadData (uint8_t cmd, uint8_t *data, uint8_t len)
{
	uint8_t n;
	
	SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
	delay_us (1);
	spiXmit (cmd);
	for (n = 1; n <= len; n++) *(data + len - n) = spiXmit (0x00);	
	SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	delay_us (1);
}

uint8_t nrfGetStatus (void)
{
	uint8_t n;
	
	SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
	delay_us (1);
	n = spiXmit (0xaa);	
	SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	delay_us (1);
	
	return n;
}	

void nrfSetChannel (uint8_t channel)
{
	// Максимальное значение канала - 127
	if (channel > 127) channel = 127;
	nrfWriteReg (NRF_REG_CHANNEL, channel);
}

void nrfSetTxAddress (uint8_t *addr)
{
	uint8_t bAddr[5];
	bAddr[0] = addr[4];
	bAddr[1] = addr[3];
	bAddr[2] = addr[2];
	bAddr[3] = addr[1];
	bAddr[4] = addr[0];
	
	nrfWriteRegMulti (NRF_REG_RXADDR_P0, bAddr, 5);
	nrfWriteRegMulti (NRF_REG_TXADDR, bAddr, 5);
	
}

void nrfSetMasterRxAddr (uint8_t *addr, uint8_t rxLen, uint8_t *mask)
{
	uint8_t bAddr[5];
	bAddr[0] = addr[4];
	bAddr[1] = addr[3];
	bAddr[2] = addr[2];
	bAddr[3] = addr[1];
	bAddr[4] = addr[0];

	*mask |= NRF_Pipe1;
	
	nrfWriteRegMulti (NRF_REG_RXADDR_P1, bAddr, 5);
	nrfWriteReg (NRF_REG_ENSHOCKBST, *mask);
	nrfWriteReg (NRF_REG_ENRXADDR, *mask);
}

void nrfPipeAdd (uint8_t pipe, uint8_t addr, uint8_t *mask)
{
	// Номер пайпа не может быть меньше 2 и больше 5
	if ((pipe < 2) || (pipe > 5)) return;
	
	*mask |= (1 << pipe);
	nrfWriteReg (NRF_REG_RXADDR_P0 + pipe, addr);
	nrfWriteReg (NRF_REG_ENSHOCKBST, *mask);
	nrfWriteReg (NRF_REG_ENRXADDR, *mask);
}

void nrfPipeDel (uint8_t pipe, uint8_t *mask)
{
	if ((pipe < 2) || (pipe > 5)) return;
	
	*mask &=~ (1 << pipe);
	nrfWriteReg (NRF_REG_ENSHOCKBST, *mask);
	nrfWriteReg (NRF_REG_ENRXADDR, *mask);
}

void nrfTransmiteData (uint8_t *data, uint8_t len)
{
	// Загружаем данные в FIFO
	nrfWriteData (NRF_CMD_WRITE_TX, data, len);
	
	// Начинаем отправку
	nrfWriteReg (NRF_REG_CONFIG, (RF_CFG_REG_VAL &~(0x01)));
}
