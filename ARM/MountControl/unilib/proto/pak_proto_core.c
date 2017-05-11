#include "include.h"

STATIC_ASSERT(PAK_PROTO_PREFIX_SIZE==3);

#define hoffset(field)  offsetof(pak_proto_frame_header_t,field)

//=============================================================================
void pak_proto_init(pak_proto_handle_t* h) // at25df_init() should be called fefore
{
	pak_proto_master_init(); // at25df_init() should be called fefore

	uart_flush(h->uart = &PAK_PROTO_UART);
}

//=============================================================================
static int pak_proto_read_header_fields(pak_proto_frame_header_t* hd, uint8_t* buf)
{
	*hd = *(pak_proto_frame_header_t*)buf;

	uint_fast8_t address = hd->address & ~PAK_PROTO_ACK_BIT_MASK;

	return (    (    (address == PAK_PROTO_LIMB_ADDRESS)
	              || (address == PAK_PROTO_NPU_ADDRESS)
				  || (address == PAK_PROTO_BROADCAST_ADDRESS))
	         && (hd->data_size == (uint8_t)~hd->not_data_size));
}

//=============================================================================
static void pak_proto_resync_frame(pak_proto_handle_t* h)
{
	if (h->idx)
	{
		if (h->resync_qty==0)
		{
			h->resync_buf = &h->buf[1];  // init resync buf
		}
		else
		{
			h->resync_buf -= (h->idx - 1); // ajust resync buf
		}

		h->resync_qty += (h->idx - 1);     // skip one byte qty
		h->idx         = 0;
	}

	h->state = 0; // IDLE
}

//=============================================================================
static int pak_proto_read_byte(pak_proto_handle_t* h, uint8_t* dst)
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

		if (x<0)
		{
			return (0);
		}

		*dst = (uint8_t)x;
		return (1);
	}
}


//=============================================================================
static int pak_proto_rx_frame(pak_proto_handle_t* h)
{
	enum  // STATES
	{
		IDLE          = 0,
		HEADER_PREFIX = 1,
		HEADER_BODY   = 2,
		DATA_AND_CRC  = 3
	};

	uint8_t x;

	while (pak_proto_read_byte(h, &x))
	{

//pin_toggle(PIN_LED_BLUE);
//dprintf("0x%02X\n",x);

		if ((h->state==IDLE) || (h->idx>=PAK_PROTO_MAX_FRAME_SIZE))
		{
			h->idx = 0;
		}

		h->buf[ h->idx ] = x;

		h->idx++;

		switch (h->state)
		{
			case IDLE:
			case HEADER_PREFIX:
			{
				if (x != (uint8_t)PAK_PROTO_PREFIX_STR[h->idx-1])
				{
                    pak_proto_resync_frame(h);
				}
				else
				{
					h->state = (h->idx < PAK_PROTO_PREFIX_SIZE) ?  HEADER_PREFIX : HEADER_BODY;

					#if 0  // 1 to debug

						if (h->state==HEADER_BODY)
						{
							static int i;
//							dprintf("prefix:%d\n",i++);
						}

					#endif
				}
				break;
			}

			case HEADER_BODY:
			{

				if (h->idx>=PAK_PROTO_HEADER_SIZE)
				{
					if (pak_proto_read_header_fields(&h->header, &h->buf[0]))  {h->state = DATA_AND_CRC; /*dprintf("header\n");*/}
					else                                                       pak_proto_resync_frame(h);
				}
				break;
			}
		
			case DATA_AND_CRC:
			{
				int field_size = h->header.data_size + PAK_PROTO_CRC_SIZE;

				if (h->idx >= PAK_PROTO_HEADER_SIZE + field_size)
				{
					h->state = IDLE;

					if (crc16_ccitt(&h->buf[PAK_PROTO_PREFIX_SIZE], PAK_PROTO_HEADER_SIZE-PAK_PROTO_PREFIX_SIZE+field_size) == 0)
					{
						#if 0  // 1 to debug

//							dprintf("crc-ok\n");

							if (h->header.frame_type==PAK_PROTO_FRAME_PHOTO_TARGRT_LINE)
							{
								for (int i=0; i<h->header.data_size; i++)
								{
//									dprintf("%02x ", h->buf[PAK_PROTO_HEADER_SIZE+i]);
								}
							}

						#endif

						return (1);
					}

					pak_proto_resync_frame(h);
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
void pak_proto_task(pak_proto_handle_t* h)
{
	pak_proto_master_task(h);

	if (h && h->uart && pak_proto_rx_frame(h))
	{
		uint_fast8_t address = h->header.address;

//		dprintf("PAK RX  FRAME: TYPE=0x%02x NUMBER=0x%08x\n", h->header.frame_type, h->header.frame_number);

		if (address == PAK_PROTO_NPU_ADDRESS)
		{
			pak_proto_master_frame_parser(h);
		}
		else if (address == PAK_PROTO_LIMB_ADDRESS | PAK_PROTO_ACK_BIT_MASK)
		{
			pak_proto_slave_frame_parser(h);
		}
	}
  }

//=============================================================================
void pak_proto_core_tx_frame(pak_proto_handle_t* h, pak_proto_frame_header_t* header, const void* data)
{
	size_t data_size = header->data_size;

	if (data && (data_size <= PAK_PROTO_MAX_DATA_SIZE))
	{
		header->not_data_size = (uint8_t)~data_size;

		memcpy(&header->prefix[0], PAK_PROTO_PREFIX_STR, PAK_PROTO_PREFIX_SIZE);

		uart_put_block(h->uart, header, PAK_PROTO_HEADER_SIZE);
		uart_put_block(h->uart, data, data_size);

		STATIC_ASSERT(offsetof(pak_proto_frame_header_t,data_size)==PAK_PROTO_PREFIX_SIZE);

		uint16_t crc = crc16_ccitt(&header->data_size, PAK_PROTO_HEADER_SIZE-PAK_PROTO_PREFIX_SIZE); // header's crc w/o prefix

		crc = crc16_ccitt_append(crc, data, data_size);

		crc = (uint16_t)(crc<<8) | (crc>>8);  // swap hi&lo bytes

		uart_put_block(h->uart, &crc, PAK_PROTO_CRC_SIZE);

		if (header->frame_type==PAK_PROTO_FRAME_PHOTO_NEW_PICTURE)
		{
//			dprintf("PAK TX FRAME: TYPE=0x%02x NUMBER=0x%08x\n", header->frame_type, header->frame_number);



/*
			dprintx(header->data_size);
			dprintx(header->not_data_size);
			dprintx(header->address);
			dprintx(header->frame_type);
			dprintx(header->frame_number);

			const pic_info_full_t* pic_info = data;

			dprintx(pic_info->id.point);
			dprintx(pic_info->id.region);
*/


		}


	}
}
