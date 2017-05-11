#include "include.h"


STATIC_ASSERT(ECU_PROTO_PREFIX_SIZE==3);


//=============================================================================
void ecu_proto_init(ecu_proto_handle_t* h)
{
	uart_flush(h->uart = &ECU_PROTO_UART);
		
}

//=============================================================================
static void ecu_proto_resync_frame(ecu_proto_handle_t* h)
{
	if (h->idx)
	{
		if (h->resync_qty==0)
		{
			h->resync_buf = &h->buf_rx[1];  // init resync buf
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
static int ecu_proto_read_byte(ecu_proto_handle_t* h, uint8_t* dst)
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
static int ecu_proto_rx_frame(ecu_proto_handle_t* h)
{
	enum  // STATES
	{
		IDLE          = 0,
		HEADER_PREFIX = 1,
		HEADER_SIZE   = 2,
		ALL_AND_CRC32  = 3
	};

	uint8_t x;

	while (ecu_proto_read_byte(h, &x))
	{
		if ((h->state==IDLE) || (h->idx>=ECU_PROTO_MAX_DATA_SIZE))
		{
			h->idx = 0;			
		}

		h->buf_rx[ h->idx++ ] = x;			
		
		switch (h->state)
		{
			case IDLE:
			case HEADER_PREFIX:
			{
				if (x != (uint8_t)ECU_PROTO_PREFIX_STR[h->idx-1])
				{
                    ecu_proto_resync_frame(h);
				}
				else
				{					
					h->state = (h->idx < ECU_PROTO_PREFIX_SIZE) ?  HEADER_PREFIX : HEADER_SIZE;
					//dprintf("P = %d,", h->idx);
				}
				break;
			}

			case HEADER_SIZE:
			{
				//dprintf("H = %d,", h->idx);
				
				if (h->idx > (offsetof(ecu_proto_frame_header_t, not_all_size) + sizeof(h->header.not_all_size) - 1) ) // ptr to next pose after not_data_size
				{
					uint16_t all_size     = read_word_lendian(&h->buf_rx[offsetof(ecu_proto_frame_header_t,     all_size)]);
					uint16_t not_all_size = read_word_lendian(&h->buf_rx[offsetof(ecu_proto_frame_header_t, not_all_size)]);

					if (all_size == (uint16_t)~not_all_size)
					{
						h->state = ALL_AND_CRC32;						
						continue;
					}

					ecu_proto_resync_frame(h);
				}
	
				break;
			}

			case ALL_AND_CRC32:            // all_size = size w/o prefix and crc !
			{
				uint16_t field_size = read_word_lendian(&h->buf_rx[offsetof(ecu_proto_frame_header_t, all_size)]) + ECU_PROTO_CRC_SIZE + 2 + 1;
				
				//dprintf("x=%X, ", x);
				//dprintf("f=%d, ", field_size);
				
				if (h->idx >= field_size)
				{
					h->state = IDLE;									
					
					uint32_t crc32 = crc32hw(&h->buf_rx[ECU_PROTO_PREFIX_SIZE], (field_size - ECU_PROTO_PREFIX_SIZE - ECU_PROTO_CRC_SIZE) , 1);
					
					//dprintf("crc32 = %X ", crc32);
					//dprintf("h->idx = %d", h->idx);
					
					if (read_dword_lendian(&h->buf_rx[h->idx - ECU_PROTO_CRC_SIZE]) == crc32) // check frame crc32 w/o prefix bytes
					{
						//dprintf("ok");											
						
						return (1);
					}									
					
					ecu_proto_resync_frame(h);
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
static void ecu_proto_read_header_fields(ecu_proto_frame_header_t* hd, uint8_t* buf)
{
	*hd = *(ecu_proto_frame_header_t*)buf;
}


//=============================================================================
void ecu_proto_task(ecu_proto_handle_t* h)
{
	if (h && h->uart && ecu_proto_rx_frame(h))
	{			
		ecu_proto_read_header_fields(&h->header, &h->buf_rx[0]);
		ecu_proto_frame_parser(h);
		h->update = 1;
	}
	else
	{
		h->update = 0;
	}	
}

//=============================================================================
void ecu_proto_master_tx(ecu_proto_handle_t* h, ecu_proto_frame_header_t *header, const void* data)
{		
	size_t data_size = header->all_size;
	
	if (data && (data_size <= ECU_PROTO_MAX_DATA_SIZE))
	{			
		header->not_all_size	= (uint16_t)~data_size;
			
		memcpy(&header->prefix[0], ECU_PROTO_PREFIX_STR, ECU_PROTO_PREFIX_SIZE);
		
		uart_put_block(h->uart, header, sizeof(ecu_proto_frame_header_t));
		
		data_size -= (ECU_PROTO_HEADER_SIZE-ECU_PROTO_PREFIX_SIZE);
		
		uart_put_block(h->uart, data,  data_size);
		
		STATIC_ASSERT(offsetof(ecu_proto_frame_header_t,all_size)==ECU_PROTO_PREFIX_SIZE);
		
/*		
		uint32_t crc = crc32hw(((unsigned char*)&header->all_size), ECU_PROTO_HEADER_SIZE-ECU_PROTO_PREFIX_SIZE, 1); // header's crc w/o prefix, l = 9 - 3 = 6

		crc = crc32hw(data, data_size, 0);
*/
		memcpy(&h->buf_tx[0], ((unsigned char*)&header->all_size), ECU_PROTO_HEADER_SIZE-ECU_PROTO_PREFIX_SIZE);
		
		memcpy(&h->buf_tx[ECU_PROTO_HEADER_SIZE-ECU_PROTO_PREFIX_SIZE], data, data_size);
		
		uint32_t crc = crc32hw(&h->buf_tx[0], (data_size + ECU_PROTO_HEADER_SIZE - ECU_PROTO_PREFIX_SIZE), 1);
		
		memcpy(&h->buf_tx[ECU_PROTO_HEADER_SIZE-ECU_PROTO_PREFIX_SIZE + data_size], &crc, 4);
	
		uart_put_block(h->uart, &crc, ECU_PROTO_CRC_SIZE);
		
	}
}

//=============================================================================
void ecu_proto_master_prepare_header(ecu_proto_handle_t* h, ecu_proto_frame_header_t *header)
{
	size_t data_size = header->all_size;
	
	if (data_size <= ECU_PROTO_MAX_DATA_SIZE)
	{			
		header->not_all_size	= (uint16_t)~data_size;
						
		memcpy(&header->prefix[0], ECU_PROTO_PREFIX_STR, ECU_PROTO_PREFIX_SIZE);
		
		memcpy(&h->buf_tx[0], &header->prefix[0], ECU_PROTO_HEADER_SIZE);
	}
}

//=============================================================================
void ecu_proto_master_append_data(void *dst, const void* src, size_t size)
{
	uint8_t *p = dst;
	
	memcpy((p + sizeof(ecu_proto_frame_header_t)), src, size);
}

//=============================================================================
void ecu_proto_master_prepare_crc(ecu_proto_handle_t* h, size_t size)
{
	uint32_t crc = crc32hw(&h->buf_tx[ECU_PROTO_PREFIX_SIZE], size, 1);
	memcpy(&h->buf_tx[size+ECU_PROTO_PREFIX_SIZE], &crc, 4);
}
