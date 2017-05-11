#include "include.h"

#pragma pack(push, 1)

typedef struct
{
	uint32_t rec_number;  // valid only if data from rom else -1
	uint16_t frame_number;
	uint8_t  frame_type;
	uint8_t  address;
	uint8_t  data_size;	
	uint8_t  buf[PAK_PROTO_MASTER_BUF_SIZE];
} pak_cache_item_t;

#pragma pack(pop)

static struct
{
	struct
	{
		pak_cache_item_t pool[PAK_PROTO_MASTER_POOL_SIZE];
		int              idx; 
		int              usage; 
	} cache;

	uint32_t             frame_number;  // master-proto frame number

} pak_master;


//=============================================================================
void pak_proto_master_init(void) // at25df_init() should be called fefore
{
	at25fs_init();

	pak_master.frame_number = at25fs_get_next_rec_number();

//	dprintf("at25fs next rec number = 0x%08lx\n\n", pak_master.frame_number);
	
	/*pak_cache_item_t item;
				
	for(uint32_t i = 0; i < pak_master.frame_number; i++)
	{
		if (at25fs_read_rec_data(i, &item, sizeof(item)))
		{			
			photo.qty_non_load_photo++;
		}
	}*/
}

//=============================================================================
void pak_proto_master_task_2hz(pak_proto_handle_t* h) // send ram pool
{
	static int i=0;

	for (int j=0; j<PAK_PROTO_MASTER_POOL_SIZE; j++, i++)	
	{
		if (i>=PAK_PROTO_MASTER_POOL_SIZE)
		{
			i = 0;
		}
		
		//dprintf("poolds:%d\n", pak_master.cache.pool[i].data_size);
		
		if (pak_master.cache.pool[i].data_size)
		{
			pak_proto_frame_header_t header =
			{
				.data_size    = pak_master.cache.pool[i].data_size,
				.address      = pak_master.cache.pool[i].address,
				.frame_type   = pak_master.cache.pool[i].frame_type,
				.frame_number = pak_master.cache.pool[i].frame_number
			};

			pak_proto_core_tx_frame(h, &header, pak_master.cache.pool[i].buf);
					
			break;
		}
	}
}

//=============================================================================
static void pak_proto_master_add_item_to_pool(const pak_cache_item_t* item)
{
	if (pak_master.cache.usage<PAK_PROTO_MASTER_POOL_SIZE)
	{
		for (int i=0; i<PAK_PROTO_MASTER_POOL_SIZE; i++)	
		{
			if (      pak_master.cache.pool[i].data_size
			     &&  (pak_master.cache.pool[i].frame_number == item->frame_number))
			{
				return; // already in the pool
			}
		}
		
		pak_master.cache.usage++;
		
		pak_master.cache.pool[pak_master.cache.idx] = *item;

		pak_master.cache.idx = (pak_master.cache.idx + 1) % PAK_PROTO_MASTER_POOL_SIZE;

//		dprintf("add to pool, usage:%lu\n", pak_master.cache.usage);
	}
}

//=============================================================================
static void pak_proto_master_cache_item(const pak_cache_item_t* item)
{
	STATIC_ASSERT(sizeof(*item) <= AT25FS_REC_DATA_SIZE);

	if (!at25fs_append(item, sizeof(*item)))
	{
//		dprintf("err: at25fs_append\n");

		at25fs_init();

		if (!at25fs_append(item, sizeof(*item))) // retry
		{
			photo.status.errors |= PHOTO_ERROR_MASK_TELEMETRY_STORAGE_FAIL;			
			
		}
	}
	else 
	{
		photo.status.errors &= ~PHOTO_ERROR_MASK_TELEMETRY_STORAGE_FAIL;

		//dprintf("add to flash, rec_number:%lu, fr_number:%d\n", item->rec_number, item->frame_number);
	}

	pak_proto_master_add_item_to_pool(item); // second add to ram
}

//=============================================================================
void pak_proto_master_task(pak_proto_handle_t* h)   // refresh ram pool by rom data if only half
{
	if  (!at25df_busy && (pak_master.cache.usage < PAK_PROTO_MASTER_POOL_SIZE/2))
	{
		static int rec=0;

		for (int i=0; i<16; i++, rec++)	
		{
			if (rec>=AT25FS_MAX_RECS_QTY)
			{
				rec = 0;
			}

			static pak_cache_item_t item; // static is for speed up
			
			if (at25fs_read_rec_data(rec, &item, sizeof(item)))
			{
				pak_proto_master_add_item_to_pool(&item);
				
				break;
			}
		}
	}
}

//=============================================================================
static void pak_proto_master_ack_received(uint_fast16_t frame_number, uint_fast8_t frame_type_for_ack)
{
	for (int i=0; i<PAK_PROTO_MASTER_POOL_SIZE; i++)	
	{
//		dprintf("i:%d dsize:%d addr:%d frtype:%d fnum:%u\n", i, pool[i].data_size, pool[i].address, pool[i].frame_type, pool[i].frame_number);
		
		if (      pak_master.cache.pool[i].data_size
		     && ((pak_master.cache.pool[i].address & (uint8_t)~PAK_PROTO_ACK_BIT_MASK) == PAK_PROTO_NPU_ADDRESS)  // waiting for echo reply w/o ACK bit
		     &&  (pak_master.cache.pool[i].frame_type                                  == frame_type_for_ack)
		     &&  (pak_master.cache.pool[i].frame_number                                == frame_number)          )
		{

			at25fs_delete_rec(pak_master.cache.pool[i].rec_number);  // remove from rom cache

			if (pak_master.cache.usage)
			{				
				pak_master.cache.usage--;		
			}

			pak_master.cache.pool[i].data_size = 0;                  // remove from ram cache
			
			photo.qty_non_load_photo--;
					
			pak_proto_master_tx_info_qty_non_load_photo(photo.qty_non_load_photo);
			
			
			//dprintf("Rx ack on frame: %lu delete rec: %lu \n", frame_number, pak_master.cache.pool[i].rec_number);

			return;
		}	
	}

}

//=============================================================================
void pak_proto_master_tx_frame_with_ack(uint_fast8_t frame_type, void* data, size_t data_size)
{
	if (data && data_size && (data_size<=PAK_PROTO_MASTER_BUF_SIZE))
	{
		pak_cache_item_t item;

		item.rec_number   = at25fs_get_next_rec_number();  // put in flash
		item.frame_number = pak_master.frame_number;
		item.frame_type   = frame_type;
		item.address      = PAK_PROTO_NPU_ADDRESS | PAK_PROTO_ACK_BIT_MASK;
		item.data_size    = data_size;

		memcpy(&item.buf[0], data, data_size);

		pak_proto_master_cache_item(&item);

		pak_master.frame_number = (pak_master.frame_number+1) % AT25FS_MAX_RECS_QTY;
		
		
		//dlog_append(&item, sizeof(item));
	}
}

//=============================================================================
void pak_proto_master_tx_info_qty_non_load_photo(uint16_t number_photo)
{
	pak_proto_frame_header_t header =
	{
		.data_size    = 2,
		.address      = PAK_PROTO_NPU_ADDRESS,
		.frame_type = PAK_PROTO_FRAME_PHOTO_QTY_NON_LOAD,
		.frame_number = pak_proto_handle.header.frame_number
	};		
			
	write_word_lendian(&pak_proto_handle.buf[PAK_PROTO_HEADER_SIZE], number_photo);
	pak_proto_core_tx_frame(&pak_proto_handle, &header, &pak_proto_handle.buf[PAK_PROTO_HEADER_SIZE]);
}

//=============================================================================
void pak_proto_master_frame_parser(pak_proto_handle_t* h)
{
	pak_proto_master_ack_received(h->header.frame_number, PAK_PROTO_FRAME_PHOTO_NEW_PICTURE);
}


#ifdef ARCTICA
//=============================================================================
void pak_proto_master_tx_arctica_alt_ls1501(void)
{
	static int i = 0;
	i++;
	pak_proto_frame_header_t header =
	{
		.data_size    	= 4,
		.address      	= PAK_PROTO_NPU_ADDRESS,
		.frame_type 	= PAK_PROTO_FRAME_SEND_ALT_LS1501,
		.frame_number 	= i
	};		
			
	write_dword_lendian(&pak_proto_handle.buf[PAK_PROTO_HEADER_SIZE], arctica_log.alt_distance_meter);
	
	pak_proto_core_tx_frame(&pak_proto_handle, &header, &pak_proto_handle.buf[PAK_PROTO_HEADER_SIZE]);
}
#endif
