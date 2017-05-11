#include "include.h"



/*
* Laser distance meter LS1501
* Author: 	Pluzhnikov E.
* Last rev: 2015.02.20
* 
*/
#define FORMAT_OUTPUT_DATA_STRING 				'0'
#define FORMAT_OUTPUT_DATA_HEXADECIMAL			'1'
#define FORMAT_OUTPUT_DATA_BINARY 				'2'
	
#define OUTPUT_DATA_DIST				 		'0'
#define OUTPUT_DATA_DIST_POWER		 			'1'
#define OUTPUT_DATA_DIST_POWER_TEMPER 			'2'


enum 
{
	STRING_LENGTH 					= 15,
	BINARY_DIST_LENGTH 				= 3,
	BINARY_DIST_POWER_LENGTH 		= 4,
	BINARY_DIST_POWER_TEMP_LENGTH 	= 8,
	HEXADEM_LENGTH 					= 10
};

enum
{		
	LS1501_START_PCKT 					= 128
};

typedef struct
{
	unsigned char qty_data_out;
	unsigned char qty_data_out_string;
	unsigned char length;
	unsigned char init_finished;
}ls1501_service_t;

ls1501_service_t ls1501_service;

ls1501_log_t ls1501_log;

timeout_t ls1501_reload_data;

timeout_t ls1501_init_seq;

unsigned char arr_cmd[20];
int ls1501_task(uart_t* const uart);


/*
* For use inititalition in main loop define INDEPENDENT_INIT
*/
static uint32_t ls1501_init_independent(uart_t* const uart);

#define INDEPENDENT_INIT

//=============================================================================
void ls1501_continuous_distance_internal_on(uart_t* const uart)
{
	memset(&arr_cmd[0], 0, 20);
	arr_cmd[0] = 'D';
	arr_cmd[1] = 'T';
	arr_cmd[2] = '\r';
		
	uart_put_block(uart, &arr_cmd[0], 3);
}

//=============================================================================
/*
* max 2000 Hz, real freq data out = sense_freq/10(Saverage)
*
*
*/
void ls1501_set_measur_freq(uart_t* const uart, uint16_t sense_freq) 
{
	memset(&arr_cmd[0], 0, 20);
	int n = 0;
	arr_cmd[0] = 'M';
	arr_cmd[1] = 'F';
	arr_cmd[2] = ' ';
	
	n += sprintf((char *)&arr_cmd[3], "%d", sense_freq) + 3;
	
	arr_cmd[n++] = '\r';
	
		
	uart_put_block(uart, &arr_cmd[0], n);
}

//=============================================================================
static void ls1501_set_output_format_data(uart_t* const uart) //set strictly one option
{
	memset(&arr_cmd[0], 0, 20);
	
	arr_cmd[0] = 'S';
	arr_cmd[1] = 'D';
	arr_cmd[2] = FORMAT_OUTPUT_DATA_BINARY;
	arr_cmd[3] = ' ';
	arr_cmd[4] = ls1501_service.qty_data_out_string;
	arr_cmd[5] = '\r';
		
	uart_put_block(uart, &arr_cmd[0], 6);
}

//=============================================================================
static void ls1501_send_stop_sequence(uart_t* const uart)
{
	#define STOP_SEQUENCE 0x1b
	uart_putc(uart, STOP_SEQUENCE);
}

//=============================================================================
static int32_t  ls1501_rx_binary(uart_t* const uart)
{
	enum  // STATES
	{		
		LS1501_START_STATE = 1,
		LS1501_DATA = 2,
		LS1501_ERROR = 3
	};
		
	static int state 	= LS1501_START_STATE;
	static size_t 	len;
	int            	c;	
	static unsigned char state_error = 0;
	
	while ((c=uart_getc(uart)) >= 0)
	{
		switch (state)
		{
			case LS1501_START_STATE:
			{
				len = 0;
				if(c == 194)
				{
					state_error = 1;
					state = LS1501_ERROR;
					//dprintf("c = %d\n", c);
					continue;
				}
				else
				{	
					if(c & 0x80)//first bit in binary format must be 1.
					{
						state_error = 0; 
						c = c & 0x7F;
						ls1501_log.arr[len++] = c;
						//dprintf("c = %d\n", c);
						state = LS1501_DATA;
						continue;
					}
				}								
			}
			
			case LS1501_ERROR:
			{	
				if(state_error == 1)
				{
					if(c == 0x45)
					{
						state_error = 2;
						//dprintf("c = %d\n", c);
						continue;
					}
					else {state_error = 0; state = LS1501_START_STATE;}
				}
				if(state_error == 2)
				{
					if(c == 0x52)
					{
						state_error = 0;
						state = LS1501_START_STATE;
						//dprintf("c = %d\n", c);
						return 0;
					}
					else {state_error = 0; state = LS1501_START_STATE;}
				}
				break;
			}
			
			case LS1501_DATA:
			{					
				ls1501_log.arr[len++] = c;
				//dprintf("c = %d\n", c);
				if (len >= ls1501_service.length || len >= 20) 
				{
					state = LS1501_START_STATE;
					//dprintf("ret1\n");
					return 1;					
				}
				break;
			}
		}
	}
	//dprintf("ret-1\n");
	return -1;
}

//=============================================================================
static void ls1501_set_param_for_bin_pckt(unsigned int state)
{	

	switch(state)
	{
		case 1:
			ls1501_service.qty_data_out_string 	= OUTPUT_DATA_DIST;
			ls1501_service.length				= BINARY_DIST_LENGTH;
			break;
		case 2:
			ls1501_service.qty_data_out_string 	= OUTPUT_DATA_DIST_POWER;
			ls1501_service.length				= BINARY_DIST_POWER_LENGTH;
			break;
		case 3:
			ls1501_service.qty_data_out_string 	= OUTPUT_DATA_DIST_POWER_TEMPER;
			ls1501_service.length				= BINARY_DIST_POWER_TEMP_LENGTH;
			break;
		default:
			break;
	}

}

//=============================================================================
static int32_t ls1501_get_sense(void *src)
{
	const uint8_t* p = src;
	
	uint8_t  b0 = (*(p+2))<<1;
	uint16_t b1 = (((uint16_t)*(p+1))<<8 | b0)>>1;
	int32_t  b2 = ((uint32_t)*(p)) 	<< 14 | b1;
	
	return b2;
}



/*
* This function work with data from laser distance meter LS1501. 
* if sense == - 1:
* if mcu not rx data from ls1501 during 10 sec, mcu send 
* "start send data sequence",  after that ls1501 must send the data again.
*/
//=============================================================================
int ls1501_task(uart_t* const uart)
{
#ifdef INDEPENDENT_INIT
	if(!ls1501_service.init_finished)
	{
		ls1501_init_independent(uart);		
	}
	else
#endif
	{
		int c;
		c = ls1501_rx_binary(uart);
		if(c == 1)
		{					
			arctica_log.alt_distance_meter = ls1501_get_sense(&ls1501_log.arr[0]);
			//dprintf("alt=%d\n", arctica_log.alt_distance_meter);
			ls1501_reload_data.stop		= 1;
			ls1501_log.new_data 		= 1;
		}
		else if(c == 0)
		{
			ls1501_log.arr[0] = 0;
			ls1501_log.arr[1] = 0;
			ls1501_log.arr[2] = 0;
			arctica_log.alt_distance_meter = 0;
			///dprintf("alt=0\n");
			if(software_timer(&ls1501_reload_data))
			{
				ls1501_continuous_distance_internal_on(&uart_laser_distance_meter);		
			}
			ls1501_reload_data.stop		= 0;
		}
		return 0;
	}
	return 1;
}



/*
*This function use, because is not available management from the 2D-NPU and the default init function is
* durable. In this function initialition of ls1501 traverse in main loop and system can work.
*/
//=============================================================================
static uint32_t ls1501_init_independent(uart_t* const uart)
{
	static uint8_t state = 0;
	if(software_timer(&ls1501_init_seq))
	{
		switch(state)
		{
			case 0:
				ls1501_set_param_for_bin_pckt(1);
				ls1501_init_seq.ms = 20000;			
				state = 1;
				break;
			case 1:
				ls1501_send_stop_sequence(uart);
				ls1501_init_seq.ms = 20000;			
				state = 2;
				break;
			case 2:
				ls1501_set_measur_freq(uart, 2000); //max 2000 Hz, real freq data out = Sense/10(Saverage)
				ls1501_init_seq.ms = 5000;			
				state = 3;
				break;
			case 3:
				ls1501_set_output_format_data(uart);
				state = 4;			
				break;
			case 4:
				ls1501_continuous_distance_internal_on(uart);
			
				ls1501_init_seq.stop				= 1;
			
				ls1501_reload_data.ms 				= 10000;
				ls1501_reload_data.update_end_sense = 0;
				ls1501_reload_data.stop				= 0;
				
				ls1501_service.init_finished = 1;
				return 1;				
			default:
				break;
		}
	}	
	return 0;
}

//=============================================================================
void ls1501_init(uart_t* const uart)
{	
#ifndef INDEPENDENT_INIT
	ls1501_set_param_for_bin_pckt(0);
		
	delay_sec(20);
	ls1501_send_stop_sequence(uart);
		
	delay_sec(20);
	ls1501_set_measur_freq(uart, 2000); //max 2000 Hz, real freq data out = Sense/10(Saverage)
	delay_sec(5);
	
	ls1501_set_output_format_data(uart);
	delay_sec(5);
	
	ls1501_continuous_distance_internal_on(uart);
	
	ls1501_reload_data.ms 				= 10000;
	ls1501_reload_data.update_end_sense = 0;
	ls1501_reload_data.stop				= 0;
	
#else
	ls1501_init_seq.ms					= 1;
	ls1501_init_seq.update_end_sense	= 0;
	ls1501_init_seq.stop				= 0;
	
	ls1501_service.init_finished		= 0;
#endif

}

//=============================================================================
/*static char*  ls1501_rx_str(uart_t* const uart)
{
	enum  // STATES
	{
		LS1501_IDLE = 0,
		LS1501_DATA = 1,		
		LS1501_CRLF = 2
	};
	
	enum
	{
		//LS1501_STRLEN_MIN =   7U,  // 
		LS1501_STRLEN_MAX =  20U   // 
	};
	
	static int state 	= LS1501_DATA;
	static size_t len;
	int            c;
	static char    str[LS1501_STRLEN_MAX+1];
	
	
	while ((c=uart_getc(uart)) >= 0)
	{
		switch (state)
		{
			case LS1501_DATA:
			{							

				if ((len<LS1501_STRLEN_MAX) && isprint(c)) 
				{
					str[len++] = c;    // read data-string (w/o tokens: <CR> <LF>)
					
					break;
				}
				else
				{
					state = LS1501_CRLF;

					//return (NULL);         // reading maximum NMEA_STRLEN_MAX chars at once
				}
				
			}

			case LS1501_CRLF:
			{
				static const char crlf[2] = "\r\n"; // <CR><LF> - end of LS1501 packet
				static int i = 0;
				
				if (c==crlf[i])
				{					
					i++;
					if(i == 2)
					{
						state = LS1501_DATA;

						len = 0;
						
						i = 0;

						return &str[0];
					}					
				}
				else
				{
					state = LS1501_DATA;

					len = 0;
					
					i = 0;

					return (NULL);
				}				
				
				break;
			}			
		}
	}
	
	return (NULL);
}*/
