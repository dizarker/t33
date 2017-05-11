#include "include.h"


#ifdef BOOTLOADER

/*
*
*	
*
*
*
*
*
*
*
*/



typedef void (*pFunction)(void);
static	pFunction Jump_To_Application;
static 	uint32_t JumpAddress;

//=============================================================================
void bootloader_task(frmwr_t* const frmwr)
{
	if(frmwr->start_update_frmwr)
	{
		if(!frmwr->curr_page.already_write && frmwr->info.all_size && !frmwr->info.error)
		{
			uint32_t addr_write = frmwr->write_info.start_addr + (frmwr->curr_page.number * FRMWR_PAGE_SIZE);
			
			if(!frmwr->already_erase)
			{
				div_t help = div(frmwr->info.all_size, FLASH_PAGE_SIZE);
				for(uint32_t i = 0; i < (help.quot + 1); i++)
				{
					flash_erase_page(addr_write + i * FLASH_PAGE_SIZE);
				}
				frmwr->already_erase = 1;
			}
			if(flash_write_block((uint8_t *)addr_write, &frmwr->data[0], frmwr->write_info.qty_bytes))
			{
				frmwr->curr_page.already_write = 1;
			}
			else
			{
				frmwr->info.error = 1;
			}
		}
	}
	
	if(frmwr->goto_main_app)
	{
		__disable_irq();
		JumpAddress = *(volatile uint32_t *)(MAIN_PROGRAMM_START_ADDR + 4);
		Jump_To_Application = (pFunction)JumpAddress;
		__set_MSP(MAIN_PROGRAMM_START_ADDR);
		Jump_To_Application();
	}
}

//=============================================================================
void bootloader_frmwr_info_task(frmwr_t* const frmwr, uint32_t all_size, uint32_t curr_page, const void* frmwr_content)
{
	frmwr->info.all_size 	= all_size;
	frmwr->info.div 		= div(frmwr->info.all_size, FRMWR_PAGE_SIZE);
	
	frmwr->curr_page.number 		= curr_page;
	frmwr->curr_page.already_write 	= 0;
	
	dprintf("page:%d,a:%d\n", curr_page, frmwr->info.div.quot);
	
	if(frmwr->curr_page.number > frmwr->info.div.quot)
	{
		frmwr->info.error = 1;
		return;
	}
	
	if(frmwr->info.div.quot == frmwr->curr_page.number)
	{
		frmwr->write_info.qty_bytes = frmwr->info.div.rem;
	}
	else if(frmwr->info.div.quot > frmwr->curr_page.number)
	{
		frmwr->write_info.qty_bytes = FRMWR_PAGE_SIZE;
	}
	
	memcpy(&frmwr->data[0], frmwr_content, frmwr->write_info.qty_bytes);
}

#endif



