#include <string.h>

#include "stm32f4xx.h"
#include "nrfRadio.h"

uint8_t temp;

nrf24l01_t radio;

uint8_t myAddr[] = {0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t txAddr[] = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e};
uint8_t testaddr[] = {0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t testpayload[32] = {0xff};

uint8_t buffer[32];

int main (void)
{
	GPIO_InitTypeDef sGPIO;
	SPI_InitTypeDef sSPI;
		
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);
	
	/*
	radio.channel = 2;
	radio.PALevel = PALevel_m18dBm;
	radio.AddressWidth = AddressWidth_5bytes;
	radio.DataRate = DataRate_2Mbps;
	
	
	nrfInit (&radio);
	nrfSetMasterRxAddr (myAddr, 32, &radio.pipeMask);
	nrfSetTxAddress (txAddr);
	
	
	
	while (1)
	{
		delay_us (1000);
		
		nrfWriteData (0xe1, NULL, 0);
		nrfTransmiteData ((uint8_t*)"12345", 5);	
		CHIPEN_PORT->ODR |= (1 << CHIPEN_Pin);
		delay_us (20);
		CHIPEN_PORT->ODR &=~ (1 << CHIPEN_Pin);
		temp = nrfGetStatus ();
	}
	
	*/
	
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
	
	delay_ms (100);
	temp = nrfReadReg (NRF_REG_CONFIG);
	temp |= 0x2;
	nrfWriteReg (NRF_REG_CONFIG, temp);
	
	delay_ms (2);
	temp = nrfReadReg (NRF_REG_CONFIG);
	temp &=~ 0x01;
	nrfWriteReg (NRF_REG_CONFIG, temp);
	nrfWriteReg (NRF_REG_ENSHOCKBST, 0x00);
	nrfWriteReg (NRF_REG_RETR, 0x00);
	temp = nrfReadReg (NRF_REG_RFSETUP);
	temp |= 0x06;
	nrfWriteReg (NRF_REG_RFSETUP, temp);
	temp |= 0x10;
	nrfWriteReg (NRF_REG_RFSETUP, temp);	
	nrfWriteRegMulti (NRF_REG_TXADDR, testaddr, 5);
	nrfWriteData (NRF_CMD_WRITE_TX, testpayload, sizeof (testpayload));
	temp = nrfReadReg (NRF_REG_CONFIG);
	temp &=~ 0x08;
	nrfWriteReg (NRF_REG_CONFIG, temp);
	nrfWriteReg (NRF_REG_CHANNEL, 0x02);
	
	CHIPEN_PORT->ODR |= (1 << CHIPEN_Pin);
	delay_us (10);
	CHIPEN_PORT->ODR &=~ (1 << CHIPEN_Pin);
	
	delay_ms (1);
	CHIPEN_PORT->ODR |= (1 << CHIPEN_Pin);
	nrfWriteData (0xe3, NULL, 0);
	
	while (1)
	{
		temp = nrfReadReg (0x17);
	}
}
