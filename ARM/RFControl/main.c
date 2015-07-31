#include <string.h>

#include "stm32f4xx.h"
#include "nrfRadio.h"

uint8_t temp;

nrf24l01_t radio;

uint8_t myAddr[] = {0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t txAddr[] = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e};

int main (void)
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);
	
	radio.channel = 0;
	radio.PALevel = PALevel_m18dBm;
	radio.AddressWidth = AddressWidth_5bytes;
	radio.DataRate = DataRate_2Mbps;
	
	
	nrfInit (&radio);
	nrfSetMasterRxAddr (myAddr, 32, &radio.pipeMask);
	nrfSetTxAddress (txAddr);
	
	while (1)
	{
		delay_us (100);
		
		nrfTransmiteData ((uint8_t*)"12345", 5);	
		CHIPEN_PORT->ODR |= (1 << CHIPEN_Pin);
		delay_us (20);
		CHIPEN_PORT->ODR &=~ (1 << CHIPEN_Pin);
	}
}
