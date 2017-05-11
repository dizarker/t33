/*
 *  dwt_timer.h rev.0.0.1 2012-03-11 [initial: 2012-03-11]
 *
 * Hi resolution (32-bit) timer based on: Data Watchpoint and Trace Unit of cm3 core
 *
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 */

#ifndef _DWT_TIMER_H_
#define _DWT_TIMER_H_


#define DWT_CYCCNT    *(volatile uint32_t*)0xE0001004
#define DWT_CONTROL   *(volatile uint32_t*)0xE0001000
#define SCB_DEMCR     *(volatile uint32_t*)0xE000EDFC


void dwt_delay(uint32_t us); // microseconds

#endif
