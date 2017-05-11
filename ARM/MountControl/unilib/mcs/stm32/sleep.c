#include "include.h"

//=============================================================================
void sleep_ms(uint32_t ms)
{
	while (ms--)
	{
		delay_ms(1);
		main_task();
	}
}


//=============================================================================
/*
void sleep_ms(uint32_t ms) // 5ms resolution
{
	uint32_t end = sys_timer.tics + ms/(1000/SYS_TIMER_FRQ);

	while (sys_timer.tics!=end)
	{
		main_task();
		sleep();     // wakeup by [timers, ahrs, uarts...]
	}
}
*/
