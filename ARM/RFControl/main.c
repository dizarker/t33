#include <string.h>

#include "stm32f4xx.h"
#include "nrfRadio.h"

uint8_t temp;

nrf24l01_t radio;

uint8_t myAddr1[] = {0x01, 0x02, 0x03, 0x04, 0x05};

int main (void)
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);
	
	radio.channel = 0;
	radio.PALevel = PALevel_m18dBm;
	radio.pipe[1].enable = ENABLE;
	radio.pipe[1].pipeRxLen = 32;
	memcpy (&radio.pipe[1].pipeRxAddr, myAddr1, sizeof (myAddr1));
	
	nrfInit (&radio);
	
	while (1)
	{
		
	}
}
