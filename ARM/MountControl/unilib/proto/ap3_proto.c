#include "include.h"

STATIC_ASSERT(AP3_PROTO_PREFIX_SIZE==3);


//=============================================================================
void ap3_proto_init(ap3_proto_handle_t* h)
{
	uart_flush(h->uart = &AP3_PROTO_UART);	
}

//=============================================================================
static void ap3_proto_resync_frame(ap3_proto_handle_t* h)
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
static int ap3_proto_read_byte(ap3_proto_handle_t* h, uint8_t* dst)
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
static int ap3_proto_rx_frame(ap3_proto_handle_t* h)
{
	enum  // STATES
	{
		IDLE          = 0,
		HEADER_PREFIX = 1,
		HEADER_SIZE   = 2,
		ALL_AND_CRC8  = 3
	};

	uint8_t x;

	while (ap3_proto_read_byte(h, &x))
	{
		if ((h->state==IDLE) || (h->idx>=sizeof(ap3_proto_frame_t)))
		{
			h->idx = 0;
		}

		h->buf[ h->idx++ ] = x;

		switch (h->state)
		{
			case IDLE:
			case HEADER_PREFIX:
			{
				if (x != (uint8_t)AP3_PROTO_PREFIX_STR[h->idx-1])
				{
                    ap3_proto_resync_frame(h);
				}
				else
				{
					h->state = (h->idx < AP3_PROTO_PREFIX_SIZE) ?  HEADER_PREFIX : HEADER_SIZE;
				}
				break;
			}

			case HEADER_SIZE:
			{
				if (h->idx > offsetof(ap3_proto_frame_header_t, not_all_size)) // ptr to next pose after not_data_size
				{
					uint8_t all_size     = h->buf[offsetof(ap3_proto_frame_header_t,     all_size)];
					uint8_t not_all_size = h->buf[offsetof(ap3_proto_frame_header_t, not_all_size)];

					if (all_size == (uint8_t)~not_all_size)
					{
						h->state = ALL_AND_CRC8;
						continue;
					}

					ap3_proto_resync_frame(h);
				}
	
				break;
			}

			case ALL_AND_CRC8:            // all_size = size w/o prefix and crc !
			{
				if (h->idx >= AP3_PROTO_PREFIX_SIZE + h->buf[offsetof(ap3_proto_frame_header_t, all_size)] + AP3_PROTO_CRC_SIZE)   // prefix + all_size + 1 byte crc8
				{
					h->state = IDLE;

					if (crc8(&h->buf[AP3_PROTO_PREFIX_SIZE], h->idx - AP3_PROTO_PREFIX_SIZE) == 0) // check frame crc8 w/o prefix bytes
					{
						return (1);
					}

					ap3_proto_resync_frame(h);
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
void ap3_proto_slave_task(ap3_proto_handle_t* h)
{
	static int ap3_tics = 0;
	if (h && h->uart && ap3_proto_rx_frame(h))
	{
		ap3_proto_frame_parser((ap3_proto_frame_t*)h->buf);
		
		h->data_rx = 1;
		ap3_tics = systick_timer.tics;	
	}
	else if((systick_timer.tics - ap3_tics) > 200)
	{
		h->data_rx = 0;		
	}
	
}
