#include "include.h"

static const uint32_t TICKS = (F_CPU) / SYSTICK_TIMER_FRQ;

systick_timer_t systick_timer;

//=============================================================================
void __irq SysTick_Handler(void) // ISR
{ 
	static int cnt;
	static int cnt2;

	if (++cnt >= SYSTICK_TIMER_FRQ)
	{
		cnt = 0;
		systick_timer.sec++;

	}

	systick_timer.tics++;

	systick_timer.updated = 1;
	
	
	cnt2++;
	systick_timer.updated_1kHz = 1;
	if (!(cnt2 % 500))
	{
		systick_timer.updated_2Hz  = 1;
	}
	if (!(cnt2 % 20))
	{
		systick_timer.updated_50Hz = 1;
	}
	
	#if defined(SYSTICK_TIMER_USERS_ISR)
		SYSTICK_TIMER_USERS_ISR();
	#endif
}

//=============================================================================
void systick_timer_init(void)
{
	SysTick_Config(TICKS);  // enable + int_on + core_clk
}

//=============================================================================
void systick_timer_reload(void) 
{
	SysTick->VAL = TICKS;   // is a downcounter (load top)
}

//=============================================================================
uint32_t software_timer(timeout_t* timeout)
{				
	timeout->start_sense = systick_timer.tics;
	
	if(timeout->stop)
	{
		timeout->update_end_sense = 1;
		return (0);
	}
	if(timeout->update_end_sense)
	{
		timeout->end_sense = timeout->start_sense + ( MS_TO_TICS(timeout->ms) );
		timeout->update_end_sense = 0;		
	}
	
	if(timeout->start_sense >= timeout->end_sense)
	{
		timeout->end_sense = timeout->start_sense + ( MS_TO_TICS(timeout->ms) );
		return (1);
	}
	else
	{
		return (0);
	}
	
}

//================================================================
void software_timer_start(timeout_t * timeout, uint32_t time)
{
	timeout->ms 				= time;
	timeout->stop 				= RESET;
	timeout->update_end_sense 	= SET;
	timeout->start_sense 		= 0;
	timeout->end_sense 			= 0;
}

//================================================================
uint32_t software_timer_elapsed(timeout_t * timeout)
{
	timeout->start_sense = systick_timer.tics;

	if(timeout->update_end_sense)
	{
		timeout->end_sense = timeout->start_sense + ( MS_TO_TICS(timeout->ms) );
		timeout->update_end_sense = 0;		
	}
	
	if(timeout->start_sense >= timeout->end_sense){ return 1;}
	else{return 0;}	
}


//================================================================
void software_timer_stop(timeout_t * timeout)
{
	timeout->stop = SET;
}

//================================================================
uint8_t software_timer_stop_test(timeout_t * timeout)
{
	if (timeout->stop)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

