#include "include.h"


//uint8_t rx_mass[SIZE_MASS];

//=============================================================================
uint32_t rx_all_data(uart_t* const uart)
{
	static int rx = 0;

	rx = uart_getc(uart);
	if(rx != -1)
	{	
		if(data.size != 0 && data.i == 0){data.size = 0;}
		data.mass[data.i++] = rx;		
		data.size++;
		pin_clr(PIN_BLINK_GREEN_LED);
		if(data.i == SIZE_MASS)
		{			
			write_in_file.qty_bytes = SIZE_MASS;
			data.i = 0; //massive finished;	need return size  - 1;
			return 0;
		}		
		return (data.size);
	}
	
	return 0;
}

//=============================================================================
void put_data(void)
{
	for(int i = 0; i < data.i; i++)
	{
		putchar1(data.mass[i++]);	
	}
}













