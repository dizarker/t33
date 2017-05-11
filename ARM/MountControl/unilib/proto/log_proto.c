#include "include.h"

STATIC_ASSERT(LOG_PROTO_PREFIX_SIZE==3);


//=============================================================================
void log_proto_init(log_proto_handle_t* h)
{
	uart_flush(h->uart = &LOG_PROTO_UART);
	
}

//=============================================================================
static void log_proto_resync_frame(log_proto_handle_t* h)
{
	if (h->idx)
	{
		if (h->resync_qty==0)
		{
			h->resync_buf = &h->buf[1];  // init resync buf
		}
		else
		{
			h->resync_buf -= h->idx - 1; // ajust resync buf
		}

		h->resync_qty += h->idx - 1;     // skip one byte qty
		h->idx         = 0;
	}

	h->state = 0; // IDLE
}

//=============================================================================
static int log_proto_read_byte(log_proto_handle_t* h, uint8_t* dst)
{
	if (h->resync_qty)
	{
		h->resync_qty--;

		*dst = *h->resync_buf++;

		return (1);
	}
	else
	{
		int x = uart_getc(h->uart);

		if (x>=0)
		{
			*dst = (uint8_t)x;
			return (1);
		}
	}

	return (0);
}

//=============================================================================
static int log_proto_rx_frame(log_proto_handle_t* h)
{
	enum  // STATES
	{
		IDLE          	= 0,
		HEADER_PREFIX 	= 1,
		BODY_SIZE		= 2,
		ALL  			= 3
	};

	uint8_t x;
	static uint16_t all_size     = 0;
	while (log_proto_read_byte(h, &x))
	{
		if ((h->state==IDLE) || (h->idx>=sizeof(log_proto_frame_t)))
		{
			h->idx = 0;
		}

		h->buf[ h->idx++ ] = x;

		switch (h->state)
		{
			case IDLE:
			case HEADER_PREFIX:
			{
				if (x != (uint8_t)LOG_PROTO_PREFIX_STR[h->idx-1])
				{
                    //log_proto_resync_frame(h);
					h->state = IDLE;
				}
				else
				{
					h->state = (h->idx < LOG_PROTO_PREFIX_SIZE) ?  HEADER_PREFIX : BODY_SIZE;
				}
				break;
				
			}
			
			case BODY_SIZE:
			{
				if (h->idx == offsetof(log_proto_frame_header_t, all_size) + 1) 
				{
					all_size     = h->buf[offsetof(log_proto_frame_header_t,     all_size)] + LOGGER_WITH_MAGNITOMETR_ADDITIONAL_SIZE;
					//uint8_t not_all_size = h->buf[offsetof(ap3_proto_frame_header_t, not_all_size)];

					/*if (all_size == (uint8_t)~not_all_size)
					{
						
						continue;
					}*/
					h->state = ALL;
					//log_proto_resync_frame(h);
				}
	
				break;
			}

			case ALL:            
			{
				if( h->idx == all_size + SIZE_PREFIX)
				{				
					h->state = IDLE;
				
					return (1);
				}				
				
				//log_proto_resync_frame(h);
				
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
void log_proto_slave_task(log_proto_handle_t* h)
{
	static int log_tics = 0;
		 
	log_proto_frame_t* log_proto_frame = (log_proto_frame_t*)&h->buf;
	
	if (h && h->uart && log_proto_rx_frame(h))
	{			
		log_proto_frame_parser(log_proto_frame);
		h->data_rx  = 1;
		h->new_pckt = 1;
		log_tics = systick_timer.tics;	
		pin_toggle(PIN_BLINK_GREEN_LED);
	}
	else if((systick_timer.tics - log_tics) > 200)
	{
		h->data_rx = 0;
	}	
}
