//Орлан. ЭБУ. Дроссельная заслонка.
//Журавлев ЕА

#ifndef _THROTTLE_H_
#define _THROTTLE_H_

/*
#include "throttle.h"
#include "pin_macro.h"
#include "servo.h"
#include "adclib.h"
#include "delay.h"
#include "filter.h"
#include "stm32f4xx.h"
#include "systick_timer.h"
*/

/*
FG36 INIT

#define THROTTLE_PIN_SERVO_FG36 A,10,L,ALT_OUTPUT_PUSH_PULL,SPEED_50MHZ
#define THROTTLE_PIN_CURRENT_SENSOR_ADC_FG36	C,1,L,ANALOG,SPEED_50MHZ

throttle_t throttle_fg36 =
{
	.settings.servo_position_min_us = 1050,
	.settings.servo_position_max_us = 2000,
	.settings.servo_orientation = SERVO_ORIENTATION_NORMAL,
	.settings.servo_normal_current_ma = 300,
	.settings.servo_jammed_current_ma = 700,
	.settings.throttle_diametr_mm = 9
};

pin_init(THROTTLE_PIN_SERVO_FG36);
pin_init_af(THROTTLE_PIN_SERVO_FG36, GPIO_AF_TIM1);
pin_init(THROTTLE_PIN_CURRENT_SENSOR_ADC_FG36);

throttle_init(&throttle_fg36, TIM1, 3, ADC1, 11);

*/

#define THROTTLE_SERVO_CURRENT_SENSOR_MAX_NORMAL_MV 2000 //~15 A, даже при кз столько быть не должно
#define THROTTLE_SERVO_CURRENT_SENSOR_MIN_NORMAL_MV 400 //~0,1 от V питания

//================================================================
typedef struct
{	
	struct
	{
		uint32_t servo_position_min_us;
		uint32_t servo_position_max_us;
		servo_orientation_t servo_orientation;
		uint32_t servo_normal_current_ma;
		uint32_t servo_jammed_current_ma;
		float throttle_diametr_mm;
	} settings;
	
	uint32_t servo_position_us;
	float throttle_area_mm2;

	struct
	{
		uint8_t init_lock;
		uint8_t init_jammed;
		uint8_t init_servo_current_sensor;
		uint8_t lock;
		uint8_t jammed;
		uint8_t servo_current_sensor;
	} errors;
		
	servo_t servo;

	uint32_t* servo_current_sensor_adc_val_adr;
	uint32_t servo_current_sensor_adc_val_zero;
	
	filter_arithmetic_mean_t servo_current_work_filter;
	timeout_t servo_selftest_timeout;
	
} throttle_t;

//================================================================
void throttle_init(throttle_t * throttle, TIM_TypeDef* servo_tim, uint8_t servo_tim_ch, ADC_TypeDef* servo_current_adc, uint8_t servo_current_adc_num); //выполнение до 30 с

void throttle_set_accel(throttle_t * throttle, uint8_t accel); //0-100%
void throttle_set_servo_position_us(throttle_t * throttle, uint32_t position_us); 

void throttle_set_servo_max_position_us(throttle_t * throttle, uint32_t position_us); 
void throttle_set_servo_min_position_us(throttle_t * throttle, uint32_t position_us); 

void throttle_task(throttle_t * throttle);

#endif
