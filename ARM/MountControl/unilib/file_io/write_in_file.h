#ifndef _WRITE_IN_FILE_H_
#define _WRITE_IN_FILE_H_

#include "write_in_file_conf.h"

#define MAX_SIZE_FILE 	0xFFFFFF00UL

typedef struct
{
	volatile uint32_t qty_bytes;	
	uint8_t *tail;	
	uint16_t curr_index;
	int16_t tail_size;
	uint8_t* dst;
	const uint8_t* src;
}write_in_file_t;

void write_in_file_flush(write_in_file_t* f);

void write_in_file_task(const void *src);

extern write_in_file_t write_in_file;




#endif
