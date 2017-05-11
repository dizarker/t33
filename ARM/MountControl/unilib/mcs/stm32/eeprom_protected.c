#include "include.h"


typedef struct
{
	uint32_t for_write;
	uint32_t last_correct_data;
}eeprom_addr_t;

typedef struct
{
	uint8_t 	error;
	eeprom_addr_t addr;
}eeprom_settings_t;

static eeprom_settings_t eeprom_settings;

//=============================================================================
static void eeprom_check_end(eeprom_settings_t *eeprom_settings)
{
	if (!IS_VALID_FLASH_ADDRESS(eeprom_settings->addr.for_write + sizeof(eeprom)))
	{
		//dprintf("lpage:%X\n", eeprom_settings->addr.for_write);
		eeprom_settings->addr.for_write = EEPROM_FLASH_BASE;
		
		#ifdef STM32F40_41xxx
			flash_erase_page(FLASH_SECTOR_FOR_WRITE);
		#else
			flash_erase_page(EEPROM_FLASH_BASE);
		#endif
	}
}

//=============================================================================
void eeprom_protected_init(void)
{
	STATIC_ASSERT(sizeof(eeprom)<=EEPROM_MAX_SIZE);
	
	eeprom_settings.addr.for_write 			= EEPROM_FLASH_BASE;
	eeprom_settings.addr.last_correct_data 	= EEPROM_FLASH_BASE;
	
	uint32_t crc 					= 0xFFFFFFFF;	
	
	memcpy(&eeprom, (void*)EEPROM_FLASH_BASE, sizeof(eeprom));	
	
	if(sizeof(eeprom) != eeprom.length)
	{
		memset(&eeprom, 0, sizeof(eeprom));
		#ifdef STM32F40_41xxx
			flash_erase_page(FLASH_SECTOR_FOR_WRITE);
		#else
			flash_erase_page(EEPROM_FLASH_BASE);
		#endif
		//dprintf("diff length. Flash erase\n");
	}
	else
	{
		for(uint32_t i = EEPROM_FLASH_BASE; i < (EEPROM_FLASH_BASE + EEPROM_MAX_SIZE); i += sizeof(eeprom))
		{
			memcpy(&eeprom, (void*)i, sizeof(eeprom));
			crc = crc32hw(&eeprom, sizeof(eeprom) - 4, 1);
			if(eeprom.crc32 != crc)
			{				
				dprintf("bad crc sector:%X,correct_sec:%X\n", i, eeprom_settings.addr.last_correct_data);
				memset(&eeprom, 0xFF, sizeof(eeprom)); 
				int res = memcmp((void*)i, (void*)&eeprom, sizeof(eeprom));
				if(res == 0)
				{
					//dprintf("dummy sector:%X,correct_sec:%X\n", i, eeprom_settings.addr.last_correct_data);
					memcpy(&eeprom, (void*)eeprom_settings.addr.last_correct_data, sizeof(eeprom));
					eeprom_settings.addr.for_write = i;
					eeprom_check_end(&eeprom_settings);
					break;
				}
			}
			else
			{
				eeprom_settings.addr.last_correct_data = i;
			}
		}
	}
	
	eeprom.length = sizeof(eeprom);
}

//=============================================================================
int eeprom_protected_sync(void)
{
	int i = 3; // retries qty

#ifndef NDEBUG	
	static int z = 0;
#endif
	eeprom_check_end(&eeprom_settings);
		
	eeprom.crc32 = crc32hw(&eeprom, sizeof(eeprom) - 4, 1);

	while (!flash_write_block((void*)eeprom_settings.addr.for_write, (void*)&eeprom, sizeof(eeprom))  &&  --i) {;}
	
	eeprom_settings.addr.for_write += sizeof(eeprom);
		
#ifndef NDEBUG		
	dprintf("addr:%X, i:%d\n", eeprom_settings.addr.for_write, z++);
#endif	
	return (!i);
}
