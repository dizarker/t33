#include "include.h"




#if defined(WRITE_IN_FILE_ARR_SIZE) && defined(WRITE_IN_FILE_TAIL_SIZE) && ((WRITE_IN_FILE_ARR_SIZE>0) || (WRITE_IN_FILE_TAIL_SIZE>0))

static uint8_t 	write_in_file_buffer[MAX(1,WRITE_IN_FILE_ARR_SIZE)];       
static uint8_t  	write_in_file_tail_buffer[MAX(1,WRITE_IN_FILE_TAIL_SIZE)];


write_in_file_t write_in_file = 
{
	.tail 	= &write_in_file_tail_buffer[0],
	.dst	= &write_in_file_buffer[0]
};

#endif

UINT bw = 0;	

//=============================================================================
void write_in_file_flush(write_in_file_t* f)
{
	memset(&f->dst[0], 0, WRITE_IN_FILE_ARR_SIZE);
	memset(&f->tail[0], 0, WRITE_IN_FILE_TAIL_SIZE);
	f->curr_index 	= 0;
	f->tail_size 	= 0;	
}

#if 0
//=============================================================================
static int pack_data_to_array_for_write(write_in_file_t* f, const void *src)
{	
	const uint8_t* ptr = src;
	
	STATIC_ASSERT(WRITE_IN_FILE_ARR_SIZE>=SIZE_MASS);
	
	if(WRITE_IN_FILE_ARR_SIZE < f->qty_bytes)
	{
		return 2;
	}
	
	if(f->tail_size > 0)
	{
		memcpy(&f->dst[0], &f->tail[0], f->tail_size);
		f->curr_index = f->tail_size;
	}
	
	f->tail_size = f->curr_index + f->qty_bytes - WRITE_IN_FILE_ARR_SIZE;
		
	if(f->tail_size < 0)
	{
		memcpy(&f->dst[f->curr_index], &ptr[0], f->qty_bytes);
		f->curr_index += f->qty_bytes;
		return 1;
	}
	else
	{
		memcpy(&f->dst[f->curr_index], &ptr[0], f->qty_bytes - f->tail_size);
		memcpy(&f->tail[0], &ptr[f->qty_bytes - f->tail_size], f->tail_size);
		f->curr_index = 0;
		return 0;
	}
}
#endif

//=============================================================================
void write_in_file_task(const void *src)
{					
	if( file_1.fsize <= MAX_SIZE_FILE )
	{		
		pin_clr(PIN_BLINK_RED_LED);
				
		res_glob = f_write(&file_1, src, write_in_file.qty_bytes, &bw);
		
		static int i = 0;
		i++;
		
		if(i == 1)
		{
			res_glob = f_sync(&file_1);
			i = 0;
		}
		if(f_s.free_clust == 0){res_glob = FR_DENIED;}
		
		pin_set(PIN_BLINK_RED_LED);
					
		#ifdef TEST_LOGGER
			#warning TEST LOGGER COMPILATION
			reset_sequence();
		#endif
		
	}
	else
	{
		reset_sequence();//need for create new file
	}
}
