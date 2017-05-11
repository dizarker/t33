/*
 * stm32f10xxx or stm32f10xxx eeprom on flash memory emulation.
 *
 */


//todo: сделать две страницы и контроль целостности по CRC или три страницы с медианной фильтрацией

#ifndef _EEPROM_H_
#define _EEPROM_H_



#ifdef STM32F40_41xxx
	#define EEPROM_MAX_SIZE    FLASH_PAGE_SIZE
	#define EEPROM_FLASH_BASE  FLASH_PAGE_ADDRES(6) //from 0 to 3 - 16kB, 0 - 64kB, 0 to 6 - 128 - kbyte, if you change page number page flash/configure flash/target/on-chip IROM1 and IROM2
#else
	#define EEPROM_MAX_SIZE    FLASH_PAGE_SIZE
	#define EEPROM_FLASH_BASE  FLASH_PAGE_ADDRES(127) //from 0 to 127 - 1kB
#endif
void eeprom_init(void);
int  eeprom_sync(void);

#endif
