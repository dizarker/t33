#include "nrfRadio.h"

void nrfInit (void)
{
	GPIO_InitTypeDef sGPIO;
	SPI_InitTypeDef sSPI;
	
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);
	
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
}

uint8_t spiXmit (uint8_t byte)
{
	uint8_t temp;
	
	SPIx->DR = byte;
	while (!(SPIx->SR &= SPI_SR_TXE));
	
	temp = SPIx->DR;
	
	return temp;
}

void _delay_us (void)
{
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
}

