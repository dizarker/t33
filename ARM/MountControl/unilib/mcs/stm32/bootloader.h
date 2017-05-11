#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

/*
*
*	If you change MAIN_PROGRAMM_START_ADDR, you must change Flash-Configure-Target - IROM1 area.
*
*
*
*/


#define FRMWR_PAGE_SIZE 128

typedef struct
{
	struct
	{
		uint32_t 	all_size;		
		uint8_t  	error;
		div_t 		div;
	}info;
	
	struct
	{
		uint32_t 	number;
		uint32_t 	already_write;
	}curr_page;
	
	struct
	{
		uint32_t 	start_addr;
		uint8_t 	qty_bytes;
	}write_info;
	
	uint8_t data[FRMWR_PAGE_SIZE];
	uint8_t start_update_frmwr;
	uint8_t goto_main_app;
	uint8_t reset;
	uint8_t already_erase;
}frmwr_t;


void bootloader_task			(frmwr_t* const frmwr);
void bootloader_frmwr_info_task	(frmwr_t* const frmwr, uint32_t all_size, uint32_t curr_page, const void* frmwr_content);
#endif
