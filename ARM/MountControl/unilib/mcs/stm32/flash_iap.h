/*
 * stm32f10xxx flash memory in application programming module (according to pm0042).
 *
 * Notes:
 *     During a write operation to the Flash memory, any attempt to read the Flash memory will stall the bus.
 * The read operation will proceed correctly once the write operation has completed.
 * This means that code or data fetches cannot be made while a write/erase operation is ongoing.
 * For write and erase operations on the Flash memory (write/erase), the internal RC oscillator (HSI) must be ON.
 * In the low-power modes, all Flash memory accesses are aborted.
 *
 */

#ifndef _FLASH_IAP_H_
#define _FLASH_IAP_H_

#if defined (STM32F10X_LD) || (STM32F10X_LD_VL) || (STM32F10X_MD) || (STM32F10X_MD_VL) || (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)
	#define MCU_LINE              MCU_LINE_MED_DENSITY     // select mcu line of:
	//=============================================================================
	#define MCU_LINE_LOW_DENSITY  1
	#define MCU_LINE_MED_DENSITY  2
	#define MCU_LINE_CONNECT      3
	#define MCU_LINE_HI_DENSITY   4


	#if  (MCU_LINE==MCU_LINE_LOW_DENSITY)
	#	define FLASH_SIZE         (32UL*1024UL)  // 32k max
	#	define FLASH_PAGE_SIZE    1024UL
	#elif (MCU_LINE==MCU_LINE_MED_DENSITY)
	#	define FLASH_SIZE         (128UL*1024UL) // 128k max
	#	define FLASH_PAGE_SIZE    1024UL
	#elif (MCU_LINE==MCU_LINE_CONNECT)
	#	define FLASH_SIZE         (128UL*2048UL) // 256k max
	#	define FLASH_PAGE_SIZE    2048UL
	#elif (MCU_LINE==MCU_LINE_HI_DENSITY)
	#	define FLASH_SIZE         (256UL*2048UL) // 512k max
	#	define FLASH_PAGE_SIZE    2048UL
	#else
	#	error WRONG MCU_LINE!!!
	#endif
	
	#define FLASH_START              0x08000000UL	
	
#elif defined (STM32F40_41xxx)
/*	
*	1Mbyte single bank Bank organization describe on 75 page.
*	I use for eeprom flash sector 1.
*	
*/	
	#define FLASH_SECTOR_NUMBER_0     0x0000	//16kb		0 sector
	#define FLASH_SECTOR_NUMBER_1     0x0008	//16kb		1 sector
	#define FLASH_SECTOR_NUMBER_2     0x0010	//16kb		2 sector
	#define FLASH_SECTOR_NUMBER_3     0x0018	//16kb		3 sector
	
	#define FLASH_SECTOR_NUMBER_4     0x0020	//64kb		0 sector
	
	#define FLASH_SECTOR_NUMBER_5     0x0028	//128kb		0 sector
	#define FLASH_SECTOR_NUMBER_6     0x0030	//128kb		1 sector
	#define FLASH_SECTOR_NUMBER_7     0x0038	//128kb		2 sector
	#define FLASH_SECTOR_NUMBER_8     0x0040	//128kb		3 sector
	#define FLASH_SECTOR_NUMBER_9     0x0048	//128kb		4 sector
	#define FLASH_SECTOR_NUMBER_10    0x0050	//128kb		5 sector
	#define FLASH_SECTOR_NUMBER_11    0x0058	//128kb		6 sector
	
	#define FLASH_SECTOR_FOR_WRITE FLASH_SECTOR_NUMBER_1
	
	#if (FLASH_SECTOR_FOR_WRITE == FLASH_SECTOR_NUMBER_0 || FLASH_SECTOR_FOR_WRITE == FLASH_SECTOR_NUMBER_1 || FLASH_SECTOR_FOR_WRITE == FLASH_SECTOR_NUMBER_2 || FLASH_SECTOR_FOR_WRITE == FLASH_SECTOR_NUMBER_3)
	
		#define FLASH_START         0x08000000UL
		#define FLASH_PAGE_SIZE    (16*1024UL)
		#define FLASH_PAGES_QTY     4
		#define FLASH_SIZE         (FLASH_PAGES_QTY*FLASH_PAGE_SIZE)		
		
	#elif (FLASH_SECTOR_FOR_WRITE == FLASH_SECTOR_NUMBER_4)
	
		#define FLASH_START         0x08010000UL
		#define FLASH_PAGE_SIZE    (64*1024UL)
		#define FLASH_PAGES_QTY     1
		#define FLASH_SIZE         (FLASH_PAGES_QTY*FLASH_PAGE_SIZE)
		
	#elif (FLASH_SECTOR_FOR_WRITE >= FLASH_SECTOR_NUMBER_5)
	
		#define FLASH_START         0x08020000UL
		#define FLASH_PAGE_SIZE    (128*1024UL)
		#define FLASH_PAGES_QTY     7
		#define FLASH_SIZE         (FLASH_PAGES_QTY*FLASH_PAGE_SIZE)
	#else
		#error FLASH_SECTOR_FOR_WRITE not define 
	#endif
			
#endif

#define FLASH_END                	(FLASH_START+FLASH_SIZE-1UL)
#define FLASH_PAGE_ADDRES(PAGE)  	((FLASH_START) + (PAGE)*(FLASH_PAGE_SIZE))
#define IS_VALID_FLASH_ADDRESS(X)   (((X) >= FLASH_START) && ((X) <= FLASH_END))

//=============================================================================
int flash_erase_page  (uint32_t page_address);            // page_address not page_number
int flash_write_word  (uint32_t address, uint16_t data);
int flash_write_block (const uint8_t* dst, const uint8_t* src, size_t len);


#endif
