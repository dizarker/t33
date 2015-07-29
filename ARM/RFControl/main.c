#include "stm32f4xx.h"
#include "nrfRadio.h"

uint8_t temp[5];

int main (void)
{
	nrfInit ();
	
	while (1)
	{
		SPI_PORT->ODR &=~ (1 << SPI_CS_Pin);
		_delay_us();
		temp[0] = spiXmit (0xff);
		SPI_PORT->ODR |= (1 << SPI_CS_Pin);
		_delay_us();
	}
}
