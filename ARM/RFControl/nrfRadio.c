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
	CHIPEN_PORT->ODR |= (1 << CHIPEN_Pin);
	
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
	nrfPipeConfig (iData->pipe);
}

uint8_t spiXmit (uint8_t byte)
{
	SPIx->DR = byte;
	while (!(SPIx->SR & SPI_SR_RXNE));	
	return SPIx->DR;
}

void delay_us (uint16_t delay)
{
	TIM4->PSC = 100;
	TIM4->ARR = delay;
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

uint8_t nrfReadReg (uint8_t addr, uint8_t value)
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
	spiXmit (addr | 0x20);
	for (n = 0; n < len; n++) *(data + n) = spiXmit (0x00);	
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

void nrfPipeConfig (pipe_t *pipe0)
{
	uint8_t pipeMask = 0, n;
	pipe_t *pnt = pipe0;
	uint8_t addr[5], regAddr, regLen;
	
	for (n = 0; n < 6; n++)
	{
		if (pnt->enable == ENABLE) 
		{
			pipeMask |= (1 << n);
	
			// Пайп заявлен как рабочий. Поэтому отправим туды адрес и пэйлоад лен
			switch (n)
			{
				case 0:
					regAddr = NRF_REG_RXADDR_P0;
					regLen = NRF_REG_RXPLEN_P0;
					break;
				case 1:
					regAddr = NRF_REG_RXADDR_P1;
					regLen = NRF_REG_RXPLEN_P1;
					break;
				case 2:
					regAddr = NRF_REG_RXADDR_P2;
					regLen = NRF_REG_RXPLEN_P2;
					break;
				case 3:
					regAddr = NRF_REG_RXADDR_P3;
					regLen = NRF_REG_RXPLEN_P3;
					break;
				case 4:
					regAddr = NRF_REG_RXADDR_P4;
					regLen = NRF_REG_RXPLEN_P4;
					break;
				case 5:
					regAddr = NRF_REG_RXADDR_P5;
					regLen = NRF_REG_RXPLEN_P5;
					break;

				default: return;
			}
			
			if (n < 2)
			{
				addr[0] = pnt->pipeRxAddr[4];
				addr[1] = pnt->pipeRxAddr[3];
				addr[2] = pnt->pipeRxAddr[2];
				addr[3] = pnt->pipeRxAddr[1];
				addr[4] = pnt->pipeRxAddr[0];			
				
				nrfWriteRegMulti (regAddr, addr, sizeof (addr));	
			}
			else nrfWriteReg (regAddr, pnt->pipeRxAddr[0]);	
			
			nrfWriteReg (regLen, pnt->pipeRxLen);
		}
		
		pnt++;
	}
		
	nrfWriteReg (NRF_REG_ENRXADDRR, pipeMask);
}
