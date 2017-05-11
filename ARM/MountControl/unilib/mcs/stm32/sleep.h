/* 
 * Sleep module rev 1.0. [24.06.2011]
 *
 */

#ifndef _SLEEP_H_
#define _SLEEP_H_

//=============================================================================
void sleep_ms(uint32_t ms); // 5ms resolution

//=============================================================================
static inline void sleep(void)
{
	__wfi();
}

#endif
