//Орлан. ЭБУ. Впускной коллектор.
//Журавлев ЕА

#ifndef _TACHOMETER_H_
#define _TACHOMETER_H_

/*
NEED

#include "atomic.h"
#include "global_macro.h"
#include "rcc.h"
#include "stm32f4xx_tim.h"
//#include "atomizer.h" //форсунка. модуля ещё нет
#include "ignition.h"
#include "tachometer.h"
#include "tachometer_irq.h"
*/

/*
FG36 INIT

#define TACHOMETR_PIN_1PULSE_FG36 B,5,L,ALT_OUTPUT_OPEN_DRAIN,SPEED_50MHZ
#define TACHOMETR_PIN_7PULSE_FG36 B,4,L,ALT_OUTPUT_OPEN_DRAIN,SPEED_50MHZ

tachometer_t tachometer_fg36;

pin_init(TACHOMETR_PIN_1PULSE_FG36);
pin_init_af(TACHOMETR_PIN_1PULSE_FG36, GPIO_AF_TIM3);

pin_init(TACHOMETR_PIN_7PULSE_FG36);
pin_init_af(TACHOMETR_PIN_7PULSE_FG36, GPIO_AF_TIM3);	

tachometer_init(&tachometer_fg36);
*/

//================================================================
typedef struct
{
	ignition_t * ignition;
	//atomizer_t atomizer; //форсунка
	
	struct
	{
		volatile uint32_t ignition_delay_us;
		volatile uint32_t atomizer_delay_us;
		volatile uint32_t atomizer_volume_pl;
	} in; //! Важно изменять: ignition_delay_us, atomizer_delay_us и atomizer_volume_pl одновременно и с запретом прерываний.
	
	struct
	{
		volatile uint32_t f_instant;
		volatile uint32_t f_average;
		uint32_t cc_value_last;
		uint8_t update_cnt;
	}sens_1pulse;
	
	struct
	{		
		uint8_t update_cnt;
	}sens_7pulse;
		
	struct
	{
		uint32_t sens_1pulse_no_work;
		uint32_t sens_7pulse_no_work;
	} errors;
	
	struct
	{
		uint32_t max_cnt;
		uint32_t f_hz;
	}
	tim_capture;
	
} tachometer_t;

//================================================================

void tachometer_init(tachometer_t* tachometer);
void tachomater_1pulse_irq(tachometer_t* tachometer, uint32_t cc_value);
void tachomater_7pulse_irq(tachometer_t* tachometer, uint32_t cc_value);
	
#endif
