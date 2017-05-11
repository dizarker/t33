#include "include.h"


#if defined (STM32F10X_LD) || (STM32F10X_LD_VL) || (STM32F10X_MD) || (STM32F10X_MD_VL) || (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)
	#pragma pack(push, 4)
	typedef struct
	{
		volatile uint32_t ACR;       // BASE + 0x00
		volatile uint32_t KEYR;      // BASE + 0x04
		volatile uint32_t OPTKEYR;   // BASE + 0x08
		volatile uint32_t SR;        // BASE + 0x0C
		volatile uint32_t CR;        // BASE + 0x10
		volatile uint32_t AR;        // BASE + 0x14
				 uint32_t _dummy;    // BASE + 0x18
		volatile uint32_t OBR;       // BASE + 0x1C
		volatile uint32_t WRPR;      // BASE + 0x20
	} fpec_t;
	#pragma pack(pop)

	/*
	#define PERIPH_BASE              ((uint32_t)0x40000000)
	#define AHBPERIPH_BASE        (PERIPH_BASE   + 0x20000)
	#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x2000)
	*/
	#define FPEC_BASE                0x40022000UL
	
	enum // Flash control register bits
	{
		FPEC_CR_PG     = (1<<0),
		FPEC_CR_PER    = (1<<1),
		FPEC_CR_MER    = (1<<2),

		FPEC_CR_OPTPG  = (1<<4),
		FPEC_CR_OPTER  = (1<<5),
		FPEC_CR_STRT   = (1<<6),
		FPEC_CR_LOCK   = (1<<7),

		FPEC_CR_OPTWRE = (1<<9),
		FPEC_CR_ERRIE  = (1<<10),

		FPEC_CR_EOPIE  = (1<<12)
	};

	enum // Flash status register bits
	{
		FPEC_SR_BSY      = (1<<0),  // FLASH Busy flag

		FPEC_SR_PGERR    = (1<<2),  // FLASH Program error flag

		FPEC_SR_WRPRTERR = (1<<4),  // FLASH Write protected error flag
		FPEC_SR_EOP      = (1<<5)   // FLASH End of Operation flag
	};


#elif defined (STM32F40_41xxx)	
	#pragma pack(push, 4)
	typedef struct
	{
		volatile uint32_t ACR;      /*!< FLASH access control register,   Address offset: 0x00 */
		volatile uint32_t KEYR;     /*!< FLASH key register,              Address offset: 0x04 */
		volatile uint32_t OPTKEYR;  /*!< FLASH option key register,       Address offset: 0x08 */
		volatile uint32_t SR;       /*!< FLASH status register,           Address offset: 0x0C */
		volatile uint32_t CR;       /*!< FLASH control register,          Address offset: 0x10 */
		volatile uint32_t OPTCR;    /*!< FLASH option control register ,  Address offset: 0x14 */
		volatile uint32_t OPTCR1;   /*!< FLASH option control register 1, Address offset: 0x18 */
	} fpec_t;
	#pragma pack(pop)

	#define FPEC_CR_PSIZE_MASK              ((uint32_t)0xFFFFFCFF)
	#define FPEC_SECTOR_MASK				((uint32_t)0xFFFFFF07)
	#define FPEC_CR_LOCK   					((uint32_t)0x80000000)
	enum // Flash control register bits
	{
		FREC_CR_PG     = (1<<0),
		FPEC_CR_SER    = (1<<1),
		FPEC_CR_MER    = (1<<2),
		
		FREC_PSIZE_HALF_WORD = (1<<8),		
		
		FPEC_CR_STRT   = (1<<16),
		FPEC_CR_EOPIE  = (1<<24)			
	};

	enum // Flash status register bits
	{
		FPEC_SR_BSY      = (1<<16),  // FLASH Busy flag

		FPEC_SR_PGERR    = (1<<6),	//FLASH Write protected error flag

		FPEC_SR_WRPRTERR = (1<<4),  // FLASH Write protected error flag
		FPEC_SR_EOP      = (1<<0)   // FLASH End of Operation flag
	};
	
	
	/*
	#define PERIPH_BASE              ((uint32_t)0x40000000)
	#define AHBPERIPH_BASE        (PERIPH_BASE   + 0x20000)
	#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x3C00)
	*/
	#define FPEC_BASE                0x40023C00UL
	
#endif


#define FPEC                     ((fpec_t*)FPEC_BASE)

//=============================================================================
int flash_write_word(uint32_t address, uint16_t data)
{
	static const uint32_t FPEC_KEY1 = 0x45670123;
	static const uint32_t FPEC_KEY2 = 0xCDEF89AB;

	if (IS_VALID_FLASH_ADDRESS(address))
	{
		ENTER_CRITICAL_SECTION();
		{	
#if defined (STM32F10X_LD) || (STM32F10X_LD_VL) || (STM32F10X_MD) || (STM32F10X_MD_VL) || (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)
			
			FPEC->KEYR = FPEC_KEY1;                 // Authorize the FPEC Access 
			FPEC->KEYR = FPEC_KEY2;

			while (FPEC->SR & FPEC_SR_BSY) {;}

			FPEC->CR = FPEC_CR_PG;                  // flash programming operation
			*(volatile uint16_t*)address = data;
			FPEC->SR;							    // according to errata: must be dummy cycle before polling BSY after START operation

			while (FPEC->SR & FPEC_SR_BSY) {;}

			FPEC->CR = FPEC_CR_LOCK; 
				
#elif STM32F40_41xxx			
			if((FPEC->CR & FPEC_CR_LOCK) != RESET)
			{
				FPEC->KEYR = FPEC_KEY1;
				FPEC->KEYR = FPEC_KEY2;
			}
			while (FPEC->SR & FPEC_SR_BSY) {;}
			
		    FPEC->CR &= FPEC_CR_PSIZE_MASK;
			FPEC->CR |= FREC_PSIZE_HALF_WORD;
			FPEC->CR |= FREC_CR_PG;
				
			*(volatile uint16_t*)address = data;
						
			while (FPEC->SR & FPEC_SR_BSY) {;}
				
			FPEC->CR &= (~FREC_CR_PG);
				
			FPEC->CR |= FPEC_CR_LOCK;
#endif			
		}
		LEAVE_CRITICAL_SECTION();
	}

	return (*(volatile uint16_t*)address == data);
}

//=============================================================================
int flash_write_block(const uint8_t* dst, const uint8_t* src, size_t len)
{

	for (const uint8_t* end=dst+len; dst<end; dst+=2)
	{
		uint16_t x;

		if ((uint32_t)dst & 1)  {x = ~0x00; dst--;}    // odd flash address - step back
		else                    {x = ~0xFF | *src++;}

		if (dst<end)  x = (x & 0xFF) | (*src++ << 8);

		if (!flash_write_word((uint32_t)dst, x))
		{
			dprintf("flash err\n");
			return 0;
		}
	}	
	return 1;
}

//=============================================================================
int flash_erase_page(uint32_t page_address) // page_address not page_number
{
	static const uint32_t FPEC_KEY1 = 0x45670123;
	static const uint32_t FPEC_KEY2 = 0xCDEF89AB;
	
	int res;

	ENTER_CRITICAL_SECTION();
	{
#if defined (STM32F10X_LD) || (STM32F10X_LD_VL) || (STM32F10X_MD) || (STM32F10X_MD_VL) || (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)		

		
		FPEC->KEYR = FPEC_KEY1;                // Authorize the FPEC Access 
		FPEC->KEYR = FPEC_KEY2;

		while (FPEC->SR & FPEC_SR_BSY) {;}

		FPEC->SR = FPEC_SR_EOP | FPEC_SR_WRPRTERR | FPEC_SR_PGERR; // clr bits by writing 1
		FPEC->CR = FPEC_CR_PER;                // page erase operation w/o fpec interrupts
		FPEC->AR = page_address;               // page address
		FPEC->CR = FPEC_CR_STRT | FPEC_CR_PER; // start operation	
		FPEC->SR;							   // according to errata: must be dummy cycle before polling BSY after START operation

		while (FPEC->SR & FPEC_SR_BSY) {;}

		res = ((FPEC->SR & FPEC_SR_EOP) != 0); // EOP is asserted at the end of each successful program or erase operation

		FPEC->CR = FPEC_CR_LOCK;               // Set the Lock Bit to lock the FPEC and the FCR
#elif STM32F40_41xxx
		
		FPEC->KEYR = FPEC_KEY1;                // Authorize the FPEC Access 
		FPEC->KEYR = FPEC_KEY2;

		while (FPEC->SR & FPEC_SR_BSY) {;}
		
		FPEC->CR &= FPEC_CR_PSIZE_MASK;
		FPEC->CR |= FREC_PSIZE_HALF_WORD; 			
	    FPEC->CR &= FPEC_SECTOR_MASK;
			
	    FPEC->CR |= FPEC_CR_SER | page_address;
		FPEC->CR |= FPEC_CR_STRT;
		
		while (FPEC->SR & FPEC_SR_BSY) {;}
			
		res = ((FPEC->SR & FPEC_SR_EOP) != 0); // EOP is asserted at the end of each successful program or erase operation
		
		/* if the erase operation is completed, disable the SER Bit */
		FPEC->CR &= (~FPEC_CR_SER);
		FPEC->CR &= FPEC_SECTOR_MASK;
		
#endif
	}
	LEAVE_CRITICAL_SECTION();

	return (res); 
}

