#include "include.h"

STATIC_ASSERT(ECU_BENZO_PROTO_PREFIX_SIZE==3);

typedef struct
{
	uint8_t already_switch_mode;
}usart_state_t;

static usart_state_t usart_state = 
{
	.already_switch_mode = 0
};

//=============================================================================
void ecu_benzo_proto_init(ecu_benzo_proto_handle_t* h)
{
	uart_flush(h->uart = &ECU_BENZO_PROTO_UART);
	#if 1
		USART_HalfDuplexCmd(ECU_BENZO_PROTO_UART.sfr, ENABLE);
	#endif
	uart_set_mode_tx(&ECU_BENZO_PROTO_UART, DISABLE);
	uart_set_mode_rx(&ECU_BENZO_PROTO_UART, ENABLE);
}

//=============================================================================
static int ecu_benzo_proto_read_byte(ecu_benzo_proto_handle_t* h, uint8_t* dst)
{

	int x = uart_getc(h->uart);

	if (x>=0)
	{
		*dst = (uint8_t)x;
		return (1);
	}

	return (0);
}

//=============================================================================
static int ecu_benzo_proto_rx_frame(ecu_benzo_proto_handle_t* h)
{
	enum  // STATES
	{
		IDLE          	= 0,
		HEADER_PREFIX 	= 1,		
		ALL  			= 2
	};

	uint8_t x;

	while (ecu_benzo_proto_read_byte(h, &x))
	{
		if ((h->state==IDLE) || (h->idx>=sizeof(ecu_benzo_proto_frame_t)))
		{
			h->idx = 0;
		}

		h->buf[ h->idx++ ] = x;

		switch (h->state)
		{
			case IDLE:
			case HEADER_PREFIX:
			{
				if (x != (uint8_t)ECU_BENZO_PROTO_PREFIX_STR[h->idx-1])
				{
                    //ecu_benzo_proto_resync_frame(h);
					h->state = IDLE;
				}
				else
				{
					h->state = (h->idx < ECU_BENZO_PROTO_PREFIX_SIZE) ?  HEADER_PREFIX : ALL;
				}
				break;
				
			}
			
			case ALL:            
			{
				if( h->idx == ECU_BENZO_PROTO_PREFIX_SIZE+ECU_BENZO_PROTO_BODY_SIZE+ECU_BENZO_PROTO_CRC_SIZE)
				{				
					h->state = IDLE;
					
					if (crc8(&h->buf[ECU_BENZO_PROTO_PREFIX_SIZE], h->idx - ECU_BENZO_PROTO_PREFIX_SIZE) == 0) // check frame crc8 w/o prefix bytes
					{
						return (1);
					}
				}							
				
				break;
			}

			default:
			{
				h->state = IDLE;
			}
		}
	}

	return (0);
}

//=============================================================================
void ecu_benzo_proto_slave_task(ecu_benzo_proto_handle_t* h)
{
	if(uart_transmission_complete(h->uart))//skip echo
	{
		if(!usart_state.already_switch_mode)
		{
			uart_set_mode_rx(h->uart, ENABLE);
			uart_set_mode_tx(h->uart, DISABLE);
			usart_state.already_switch_mode = 1;
		}
	}
	
	static int ecu_benzo_tics = 0;
	
	ecu_benzo_proto_frame_t* ecu_benzo_proto_frame = (ecu_benzo_proto_frame_t*)&h->buf;
	
	if (h && h->uart && ecu_benzo_proto_rx_frame(h))
	{			
		ecu_benzo_proto_frame_parser(ecu_benzo_proto_frame);
		h->data_rx  = 1;
		h->new_pckt = 1;
		ecu_benzo_tics = systick_timer.tics;	
	}
	else if((systick_timer.tics - ecu_benzo_tics) > 200)
	{
		h->data_rx = 0;
	}
	
}

//=============================================================================
void ecu_benzo_proto_tx(ecu_benzo_proto_handle_t* h, ecu_benzo_proto_frame_header_t* header, const void* data, size_t size)
{
	usart_state.already_switch_mode = 0;
	
	uart_set_mode_rx(h->uart, DISABLE);
	uart_set_mode_tx(h->uart, ENABLE);
	
	if (data && (size <= ECU_BENZO_PROTO_MAX_DATA_SIZE))
	{
		memcpy(&header->prefix[0], ECU_BENZO_PROTO_PREFIX_STR, ECU_BENZO_PROTO_PREFIX_SIZE);

		uart_put_block(h->uart, header, ECU_BENZO_PROTO_HEADER_SIZE);
		uart_put_block(h->uart, data, size);

		uint8_t crc = crc8(&header->packet_type, ECU_BENZO_PROTO_HEADER_SIZE-ECU_BENZO_PROTO_PREFIX_SIZE); // header's crc w/o prefix

		crc = crc8_append(crc, data, size);

		uart_put_block(h->uart, &crc, ECU_BENZO_PROTO_CRC_SIZE);
	}
}

