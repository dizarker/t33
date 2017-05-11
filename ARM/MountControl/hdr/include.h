#ifndef _INCLUDE_H_
#define _INCLUDE_H_

// Global definitions
#define F_CPU										168000000
#define HSE											16000000

// Standart C library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// STM core header
#include "stm32f4xx.h"

// Main unilib modules headers
#include "global_macro.h"
#include "static_assert.h"
#include "atomic.h"
#include "rcc.h"
#include "pin_macro.h"

// Unilib modules headers
#include "fifo.h"
#include "uart.h"
#include "systick_timer.h"
#include "delay.h"
#include "wdt.h"

// Low level headers
#include "low_level.h"

// Global variables header
#include "var.h"

#endif /* _INCLUDE_H_ */
