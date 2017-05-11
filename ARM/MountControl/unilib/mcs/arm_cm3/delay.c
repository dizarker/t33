#include "include.h"

//=============================================================================
/* User code to delay the processor goes here. Below is example code that
 *   works for a 32-bit ARM7 Cortex processor clocked at 72 MHz.  For any 
 *  other processor you will need to replace this with code that works
 *  for your processor.  Many compilers will have their own delay routines
 *  so make sure you check your compiler documentation before attempting to
 *  write your own.
 */


#if defined (STM32F10X_LD) || (STM32F10X_LD_VL) || (STM32F10X_MD) || (STM32F10X_MD_VL) || (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)

//=============================================================================
void delay_us(uint32_t us)
{
	us *= 8;

	while (us--) // 4  SUBS + BCS + PIPLINE_RELOAD
	{
		__nop(); //	5
		__nop(); //	6
		__nop(); //	7
		__nop(); //	8
		__nop(); //	9  9*8= 72 clk at MHz = 1us
	}
}

#elif defined (STM32F40_41xxx) || (STM32F427_437xx) || (STM32F429_439xx) || (STM32F401xx) || (STM32F411xE)
//=============================================================================
void delay_us(uint32_t us)
{
	us *= 21;

	while (us--) // 4  SUBS + BCS + PIPLINE_RELOAD
	{
		__nop(); //	5
		__nop(); //	6
		__nop(); //	7
		__nop(); //	8 8*21= 168 clk at MHz = 1us
	}
}
#endif
//=============================================================================
void delay_ms(uint32_t ms)
{
	delay_us(ms*1000UL);

	wdt_reset();
}

//=============================================================================
void delay_sec(uint32_t sec)
{
	while (sec--)
	{
		wdt_reset();

		delay_us(1000000UL);
	}

	wdt_reset();
}
