#include "include.h"
/*
*	laser distance meter ilm150t
*	configuration of this laser adjust in program MDL Laser. In this programm you can set different config to this laser/
*	Example: binary output format or string format output
*	
*/
uint16_t test = 0;
enum 
{	
	BINARY_LENGTH 	= 2,	
};

unsigned char arr_cmd[20];
int ilm150_t_task(uart_t* const uart);
void ilm150_t_init(void);
static uint16_t ilm150_t_get_sense(uint16_t x);

//=============================================================================
void ilm150_t_start_laser(uart_t* const uart)
{
	memset(&arr_cmd[0], 0, 20);
	arr_cmd[0] = 'N';  //0x46
	arr_cmd[1] = '\n'; //0xa
	arr_cmd[2] = '\r';	//0xd
		
	uart_put_block(uart, &arr_cmd[0], 3);
}

//=============================================================================
void ilm150_t_stop_laser(uart_t* const uart)
{
	memset(&arr_cmd[0], 0, 20);
	arr_cmd[0] = 'F';	
	arr_cmd[1] = '\n';	
	arr_cmd[2] = '\r';	
		
	uart_put_block(uart, &arr_cmd[0], 3);
}

//=============================================================================
static int32_t  ilm150_t_rx_binary(uart_t* const uart)
{
	enum  // STATES
	{
		ILM150_T_IDLE = 0,
		ILM150_T_DATA = 1,		
		ILM150_T_END_PCKT = 2
	};
	
	enum
	{
		ILM150_T_STRLEN_MAX =  10U   // 
	};
	
	static int state 	= ILM150_T_DATA;
	static size_t len;
	int            c;
	static char    bindata[ILM150_T_STRLEN_MAX+1];
	
	
	while ((c=uart_getc(uart)) >= 0)
	{
//		dprintf("c = %d\n", c);
		switch (state)
		{
			case ILM150_T_DATA:
			{							
				if((c>>6) == 2) //most significant byte
				{
					bindata[len++] = c;
					break;
				}
				else if((c>>6) == 0) //least significant byte
				{
					bindata[len++] = c;
					state = ILM150_T_END_PCKT;
				}
				else if( len >= ILM150_T_STRLEN_MAX )
				{
					len = 0;
				}
			}

			case ILM150_T_END_PCKT:
			{
																		
				state = ILM150_T_DATA;

				len = 0;

				return read_word_bendian(&bindata[0]);				
			}
		}
	}
	
	return -1;
}

//=============================================================================
int ilm150_t_task(uart_t* const uart)
{
	int32_t sense = ilm150_t_rx_binary(uart);
	#define MAXIMUM_MEASUR_DIST_DECIMETR	4095UL		
	if(sense != -1)
	{				
		test = ilm150_t_get_sense(sense);
		
		if(test == MAXIMUM_MEASUR_DIST_DECIMETR){test = 0;}
		
		memcpy(&flying_lab_ilm150.arr[0], &test,  BINARY_LENGTH);
		return 1;
	}
	else if(sense == -1)	
	{
		return 0;
	}
	return 0;
}

//=============================================================================
static uint16_t ilm150_t_get_sense(uint16_t x)
{
	uint16_t help = 0;

	help = x&0x3F;
	
	help = help | ((x&0x3F00)>>2);
	
	return help;
}

//=============================================================================
void ilm150_t_init(void)
{
	uart_init(&uart_laser_distance_meter,   115200UL);  // 115200-8-n-1 
			
	ilm150_t_start_laser(&uart_laser_distance_meter);
	delay_sec(5);
}
