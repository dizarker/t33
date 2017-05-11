#include "include.h"

at25fs_t at25fs;

//=============================================================================
static uint32_t at25fs_rec_number_to_address(uint32_t rec_number)
{
	return (AT25FS_OFFSET + rec_number*AT25FS_REC_FULL_SIZE);
}

//=============================================================================
static int at25fs_read_rec_raw(uint32_t rec_number, void* dst, size_t len, int raw_offset)
{
	if ((rec_number<AT25FS_MAX_RECS_QTY) && dst && len && ((len+raw_offset)<=AT25FS_REC_FULL_SIZE))
	{
		at25df_read(at25fs_rec_number_to_address(rec_number) + raw_offset, dst, len);

		return 1;
	}

	return 0;
}

//=============================================================================
static int at25fs_write_rec_raw(uint32_t rec_number, const void* src, size_t len, int raw_offset)
{
	if ((rec_number<AT25FS_MAX_RECS_QTY) && src && len && ((len+raw_offset)<=AT25FS_REC_FULL_SIZE))
	{
		return (at25df_write(at25fs_rec_number_to_address(rec_number) + raw_offset, src, len));
	}

	return 0;
}

//=============================================================================
static int at25fs_read_rec_header(uint32_t rec_number, at25fs_rec_header_t* ph)
{
	return at25fs_read_rec_raw(rec_number, ph, sizeof(*ph), offsetof(at25fs_rec_t, header));
}

//=============================================================================
static int at25fs_is_active_rec(at25fs_rec_header_t* header)
{      
	return (    (header->status.empty != AT25FS_TRUE)
		     && (header->status.valid == AT25FS_TRUE) );
}

//=============================================================================
static int at25fs_rec_is_writeble(at25fs_rec_header_t* header)
{      
	return (    (header->status.empty == AT25FS_TRUE)
		     && (header->status.valid == AT25FS_TRUE) );
}

//=============================================================================
int at25fs_read_rec_data(uint32_t rec_number, void* data_dst, size_t len)
{
	at25fs_rec_header_t header;
	      
	if (    at25fs_read_rec_header(rec_number, &header)
	     && at25fs_is_active_rec(&header)               )
	{
		return at25fs_read_rec_raw(rec_number, data_dst, len, offsetof(at25fs_rec_t, data)); // skip header
	}

	return 0;
}

//=============================================================================
int at25fs_delete_rec(uint32_t rec_number)
{
	at25fs_rec_header_t h;

	if (at25fs_read_rec_header(rec_number, &h) && at25fs_is_active_rec(&h))
	{
		static const uint8_t invalid = AT25FS_FALSE;

		STATIC_ASSERT(sizeof(invalid)==1);

		return at25fs_write_rec_raw(rec_number, &invalid, sizeof(invalid), offsetof(at25fs_rec_t, header.status.valid));
	}

	return 1;
}

//=============================================================================
void at25fs_delete_all_recs(void)
{
	for (int i=0; i<AT25FS_MAX_RECS_QTY; i++)
	{
		at25fs_delete_rec(i);
		
		delay_ms(1);

		wdt_reset();
	}

	at25fs.next_rec_number = 0;
}

//=============================================================================
uint32_t at25fs_get_next_rec_number(void)
{
	return (at25fs.next_rec_number);
}


//=============================================================================
void at25fs_init(void)
{
	at25fs.next_rec_number = 0;

	for (uint32_t i=0; i<AT25FS_MAX_RECS_QTY; i++)
	{
		at25fs_rec_header_t header;

		if (    at25fs_read_rec_header(i, &header)
		     && at25fs_rec_is_writeble(&header))
		{
/*
			dprintf("i=0x%08lx empty=0x%02lx valid=0x%02x\n", i, header.status.empty, header.status.valid);

			#ifndef NDEBUG
				delay_ms(10);  // used to delay debug printf (prevent tx_buf overflow)
			#endif
*/
			at25fs.next_rec_number = i;
			break;
		}
		/*else
		{
			if(at25fs_is_active_rec(&header))
			{
			}
		}*/

		wdt_reset();
	}
}

//=============================================================================
int at25fs_append(const void* data_src, size_t len)
{
	if (data_src && len && (len<=AT25FS_REC_DATA_SIZE))
	{
		uint32_t address  = at25fs_rec_number_to_address(at25fs.next_rec_number);

		if (    (address % AT25FS_BLOCK_SIZE)   // erase next block before write in to
		     || at25df_erase_block4k(address) )
		{
			at25fs_rec_t rec;

			rec.header.status.empty = AT25FS_FALSE;
			rec.header.status.valid = AT25FS_TRUE;

			memcpy(&rec.data[0], data_src, len);

			if (at25fs_write_rec_raw(at25fs.next_rec_number, &rec, sizeof(rec), 0))
			{
				at25fs.next_rec_number = (at25fs.next_rec_number + 1) % AT25FS_MAX_RECS_QTY;

				return (1);
			}
		}
	}

//	dprintf("err: at25fs_append\n");

	return (0);
}
