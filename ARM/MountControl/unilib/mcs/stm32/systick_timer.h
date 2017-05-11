/* 
 * System-timer max resolution 24-bit.
 *
 *
 */

#ifndef _SYSTICK_TIMER_H_
#define _SYSTICK_TIMER_H_

#include "systick_timer_conf.h"

#define SEC_TO_MS(A)	(A*1000UL)
#define MS_TO_TICS(A) 	(A/(1000UL / SYSTICK_TIMER_FRQ))

typedef struct
{
	volatile uint32_t tics;    // tics-timer
	volatile uint32_t sec;     // seconds-timer
	volatile uint32_t updated; // flag
	#if 1
	volatile uint32_t updated_50Hz;
	volatile uint32_t updated_2Hz;
	volatile uint32_t updated_1kHz;
	#endif
	
} systick_timer_t;


typedef struct	
{
	uint32_t start_sense; 
	uint32_t end_sense;
	uint32_t update_end_sense;
	uint32_t ms;
	uint8_t stop;
}timeout_t; // all measured in ms



extern systick_timer_t systick_timer;

void 		systick_timer_init   		(void);
void 		systick_timer_reload 		(void);
uint32_t 	software_timer				(timeout_t* timeout);
void 		software_timer_start		(timeout_t * timeout, uint32_t time);
void		software_timer_stop			(timeout_t * timeout);
uint32_t 	software_timer_elapsed(timeout_t * timeout);
uint8_t software_timer_stop_test(timeout_t * timeout);

#endif
