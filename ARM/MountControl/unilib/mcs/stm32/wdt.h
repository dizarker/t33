/* 
 * WDT (IWDG) module rev 1.0. [24.06.2011]
 *
 * IWDG is a decrement 12bit watchdog counter.
 *
 */

#ifndef _WDT_H_
#define _WDT_H_

enum
{
	WDT_KEY_START          = 0xCCCC,
	WDT_KEY_RELOAD         = 0xAAAA,
	WDT_KEY_ACCESS_ENABLE  = 0x5555
};

enum
{
	WDT_PRESCALER_4   = 0,
	WDT_PRESCALER_8   = 1,
	WDT_PRESCALER_16  = 2,
	WDT_PRESCALER_32  = 3,
	WDT_PRESCALER_64  = 4,
	WDT_PRESCALER_128 = 5,
	WDT_PRESCALER_256 = 6
};

//=============================================================================
static inline void wdt_reset(void)
{
	#ifdef NDEBUG

		IWDG->KR = WDT_KEY_RELOAD;

	#endif
}

//=============================================================================
static inline void wdt_enable(const float wdto_sec) // range [0..3.2767]sec, resolution 4096
{
	#ifdef NDEBUG

		static const uint32_t WDT_DIV = 32;
		static const uint32_t WDT_CLK = 40000;     // is the worst case, nominal vluse is 32768

		uint32_t x = wdto_sec * WDT_CLK / WDT_DIV; // reload value range [0x0000...0x0fff]

		IWDG->KR   = WDT_KEY_ACCESS_ENABLE;        // unlock PR & RLR

		IWDG->PR   = WDT_PRESCALER_32;             // init
		IWDG->RLR  = x;

		IWDG->KR   = WDT_KEY_RELOAD;               // apply & auto lock

		IWDG->KR   = WDT_KEY_START;                // start (the LSI oscillator will be enabled by hardware)

	#endif
}

/*
//=============================================================================
static inline void wdt_enable_permanent(void)
{
	int option = FLASH_GetUserOptionByte();
 
	if (option & FLASH_OBR_WDG_SW)
	{
    	FLASH_Unlock();
    	FLASH_UserOptionByteConfig(OB_IWDG_HW, OB_STOP_RST, OB_STDBY_RST);
		FLASH_Lock();
	}
}
*/

//=============================================================================
static inline void NORETURN mcu_reset(void)  // must be not macro! 
{
	NVIC_SystemReset();

	for (;;); // wait until reset
}

#endif
