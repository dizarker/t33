#include "stm32f4xx.h"
#include "nrfRadio.h"

uint8_t temp;

int main (void)
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);
		
	nrfInit ();
	
	while (1)
	{
		SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
		temp = spiXmit (0xff);
		SPI_PORT->ODR |= (1 << SPI_CS_Pin);
	}
}
