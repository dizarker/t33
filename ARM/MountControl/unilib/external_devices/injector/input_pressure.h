//Орлан. ЭБУ. Впускной коллектор.
//Журавлев ЕА

#ifndef _INPUT_PRESSURE_H_
#define _INPUT_PRESSURE_H_

/*
#include "ms5425do.h"
#include "stm32f4xx_tim.h"
#include "rcc.h"
#include "input_pressure.h"
#include "input_pressure_irq.h"
*/

/*
FG36 INIT

input_pressure_init(&input_collector_fg36);
*/

#define INPUT_PRESSURE_I2C i2c1

#define INPUT_PRESSURE_SENSOR_DIRTY_LEVEL_WAIT_CNT_MAX 60 //через сколько опросов датчика считать что он грязный, если давление не опускается ниже INPUT_COLLECTOR_PRESSURE_SENSOR_DIRTY_LEVEL_MPSI
#define INPUT_PRESSURE_SENSOR_DIRTY_LEVEL_MPSI -4000
#define INPUT_PRESSURE_IGNITION_LEVEL_MPSI -7000

#define INPUT_PRESSURE_SENSOR_VALUE_BUFFER_CNT 512 //должен быть степенью двойки
#define INPUT_PRESSURE_SENSOR_VALUE_TYPE int16_t
#define INPUT_PRESSURE_SENSOR_VALUE_SIZE 2

//================================================================
typedef struct
{
	struct
	{
		int32_t sensor_dirty_level_wait_cnt_max;
		int32_t sensor_dirty_level_mpsi;
		int32_t sensor_ignition_level_mpsi;
	
		volatile uint32_t ignition_delay_us;
		volatile uint32_t atomizer_delay_us;
		volatile uint32_t atomizer_volume_pl;
	} in;
	
	struct
	{
		uint32_t sensor_not_response;
		uint32_t sensor_dirty;
	} errors;
	
	ms5425do_t sensor;
	ignition_t * ignition;
	//atomizer_t atomizer; //форсунка	
	
	fifo_t sensor_value_mpsi_fifo;
	
	int32_t sensor_dirty_level_wait_cnt;
	uint8_t fl_action_alredy;	
} input_pressure_t;

//================================================================
void input_pressure_init(input_pressure_t* input_pressure);
void input_pressure_get_value_irq(input_pressure_t* input_pressure);


//================================================================
#ifdef MAIN

INPUT_PRESSURE_SENSOR_VALUE_TYPE sensor_value_mpsi_buffer[INPUT_PRESSURE_SENSOR_VALUE_BUFFER_CNT];

input_pressure_t input_pressure_fg36 =
{
	.in =
	{
		.sensor_dirty_level_wait_cnt_max 	= INPUT_PRESSURE_SENSOR_DIRTY_LEVEL_WAIT_CNT_MAX,
		.sensor_dirty_level_mpsi 					= INPUT_PRESSURE_SENSOR_DIRTY_LEVEL_MPSI,
		.sensor_ignition_level_mpsi 			= INPUT_PRESSURE_IGNITION_LEVEL_MPSI,
	},
	
	.sensor =
	{
		.i2c 										= &INPUT_PRESSURE_I2C,
		.i2c_f 									= 300000,
		.i2c_address 						= 0x28,
		.MS4525DO_OutputType 		= MS4525DO_OutputTypeA,
		.MS4525DO_PressureRange = MS4525DO_PressureRange15,
		.MS4525DO_PressureType 	= MS4525DO_PressureTypeDifferencial,
	},
	
	.sensor_value_mpsi_fifo =
	{
		.buffer	= (uint8_t *)sensor_value_mpsi_buffer,
		.size 	= INPUT_PRESSURE_SENSOR_VALUE_BUFFER_CNT * INPUT_PRESSURE_SENSOR_VALUE_SIZE,
		.in 		= 0,
		.out 		= 0,
	},
};

#else

extern input_pressure_t input_pressure_fg36;

#endif

#endif
