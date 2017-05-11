#include "include.h"

//=============================================================================
void eeprom_init(void)
{
	STATIC_ASSERT(sizeof(eeprom)<=EEPROM_MAX_SIZE);

	memcpy(&eeprom, (void*)EEPROM_FLASH_BASE, sizeof(eeprom));	
	
	if(eeprom.feedforward_delay.ui32 == 0xFFFFFFFF) //на случай, если запись во flash не производилась // to do: check isNAN?
	{
		eeprom.feedforward_delay.fl = 0;
	}
}

//=============================================================================
int eeprom_sync(void)
{
	int i = 3; // retries qty
		
	#ifdef STM32F40_41xxx
		flash_erase_page(FLASH_SECTOR_FOR_WRITE);
	#else 
		flash_erase_page(EEPROM_FLASH_BASE);
	#endif

	while (!flash_write_block((void*)(EEPROM_FLASH_BASE), (void*)&eeprom, sizeof(eeprom))  &&  --i) {;}

	return (!i);
}
