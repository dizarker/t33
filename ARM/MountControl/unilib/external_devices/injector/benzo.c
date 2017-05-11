#include "include.h"
/*
  Список зависимостей. 
  include <pid_simple.h>
  include <ms5425do.h>
	include <filter.h>
	include <adclib.h>
	include <stm32f4xx_tim.h>
	include <rcc.h>
*/
 

static void benzoAdc_init(ADC_TypeDef* ADCx, uint8_t channel, benzo_t* const benzo);
static void pwm_setup(benzo_t* const benzo);
static ErrorStatus check_errors(benzo_t* const benzo);
static void get_allValues(benzo_t* const benzo);
static void set_pressure(benzo_t* const benzo, uint16_t impact);
static void pressure_proc(int32_t * val, ms5425do_t * presure, uint8_t * event);
static void benzo_init(benzo_t* const benzo, TIM_TypeDef* TIMx, uint8_t tim_channel, ADC_TypeDef* ADCx, uint8_t adc_channel); 	//init

//=====global functions===========================================================
ErrorStatus benzo_task(benzo_t* const benzo) 
{			
	pressure_proc(&benzo->pressure.sensor.value_mpsi, &benzo->pressure.sensor.ms4525, &benzo->pressure.sensor.event_update);	
		
	if(software_timer(&benzo->pressure.sensor.timeout_request)) //check sensor for new data
	{
		benzo->pressure.sensor.event_update = SET;		
	}
	
	if(software_timer(&benzo->pressure.pid_timeout_request))   
	{
			//get and filter pressure and current values
			get_allValues(benzo);
																	
			benzo->impct = pid_algorithm(&benzo->pressure.pid)*benzo->pressure.pid_limiter;		//pid for pressure sensor	
						
			set_pressure(benzo, (uint16_t)benzo->impct);
			
			return check_errors(benzo);
	}
	
	return SUCCESS;
}

//=============================================================================
void benzo_set_state(benzo_t* const benzo, FunctionalState newState)
{
		TIM_Cmd(benzo->pump.pwm.tim_number, newState);	
		if(newState == ENABLE)
		{
			pin_set(PIN_C4);
		}
		else
		{
			pin_clr(PIN_C4);
		}
}

//===== init all periph ========
void benzo_init_fg36(benzo_t* const benzo)
{
		//sensor init
		pin_init(PIN_I2C3_SCL); 
		pin_init(PIN_I2C3_SDA);
		pin_init_af(PIN_I2C3_SCL, GPIO_AF_I2C3);
		pin_init_af(PIN_I2C3_SDA, GPIO_AF_I2C3);
		
		pin_init(PIN_C4);
	
		benzo->pressure.sensor.ms4525.i2c = &I2C_PRESSURE_BENZO;
		benzo->pressure.sensor.ms4525.i2c_f = 400000;
		benzo->pressure.sensor.ms4525.i2c_address = 0x28;
		benzo->pressure.sensor.ms4525.MS4525DO_OutputType = MS4525DO_OutputTypeA;
		benzo->pressure.sensor.ms4525.MS4525DO_PressureRange = MS4525DO_PressureRange100;
		benzo->pressure.sensor.ms4525.MS4525DO_PressureType = MS4525DO_PressureTypeGauge;
		ms4525do_init(&benzo->pressure.sensor.ms4525);
				
		benzo->pressure.sensor.timeout_request.ms = 1;
	
		//tim2_ch1, adc1_ch8
		benzo_init(benzo, TIM2, 1, ADC1, 8);
	
		//pressure setpoint
		
		benzo->pressure.value_filter.length = 10;
		benzo->current.value_filter.length = 10;			
		
		benzo->pressure.pid_timeout_request.ms = 20; 
		benzo->errors.pressure_sensor_timeout.ms = 20;
		
		benzo->pressure.set_value_atm = &benzo->pressure.pid.in.sp;     //уставка
		benzo->pressure.pid_init.pv = &benzo->pressure.value_atm;       //данные	
		
		pid_init(&benzo->pressure.pid, &benzo->pressure.pid_init);
		
		*benzo->pressure.set_value_atm = 0.2;
		
		benzo->pressure.pid.math.k.p = 0.15;
		benzo->pressure.pid.math.k.d = 0.8; 
		benzo->pressure.pid.math.k.i = 0.005; 
		
		benzo->pressure.pid_limiter = 1000; //max impact
		
		benzo->pressure.pid.in.lowpass.k.a = .5;
		benzo->pressure.pid.in.lowpass.k.b = .5;
	 
}



//=============================================================================
static void get_allValues(benzo_t* const benzo) 
{			
		//get middle current value
		benzo->current.value_raw = filter_hi_pass(&benzo->current.value_filter, *benzo->current.adc_val_adr);
		
	  if(benzo->errors.pressure_sensor_error == SET)
		{		
			benzo->pressure.value_atm = (benzo->current.value_raw - 705)/100;
		}
		else
		{
			//convert mpsi pressure to atm  
			benzo->pressure.value_atm = (filter_hi_pass(&benzo->pressure.value_filter, benzo->pressure.sensor.ms4525.value_mpsi))/14690.0;
		}
		
		if(benzo->pressure.sensor.ms4525.fl_error == SET)
		{
			benzo->pressure.value_atm = benzo->pressure.old_value_atm;
		}	
		
		benzo->current.value_a = (0.006f*(float)benzo->current.value_raw)-4.01f;
}			

//=============================================================================
static void set_pressure(benzo_t* const benzo, uint16_t impact)
{
		if(benzo->impct < 0) benzo->impct = 0;
		if(benzo->impct > MAX_IMPACT) benzo->impct = MAX_IMPACT;
	
		switch(benzo->pump.pwm.tim_channel)
		{
		case 1:
			benzo->pump.pwm.tim_number->CCR1 = impact;
			break;
		
		case 2:
			benzo->pump.pwm.tim_number->CCR2 = impact;
			break;
		
		case 3:
			benzo->pump.pwm.tim_number->CCR3 = impact;
			break;
		
		case 4:
			benzo->pump.pwm.tim_number->CCR4 = impact;
			break;
		
		default:
			break;
		}		
}

//=============================================================================
void benzo_init(benzo_t* const benzo, TIM_TypeDef* TIMx, uint8_t tim_channel, ADC_TypeDef* ADCx, uint8_t adc_channel) 
{		
		benzo->pressure.sensor.timeout_request.stop = 0;
		benzo->pressure.sensor.timeout_request.update_end_sense = 1;
		
		benzo->pressure.pid_timeout_request.stop = 0;
		benzo->pressure.pid_timeout_request.update_end_sense = 1;
	
		benzo->errors.pressure_sensor_timeout.stop = 1;
		benzo->errors.pressure_sensor_timeout.update_end_sense = 1;
	
		benzo->pump.pwm.tim_number = TIMx;
		benzo->pump.pwm.tim_channel = tim_channel; 	
	
		//pwm init
		pin_init(PIN_PUMP_PWM);
		pin_init_af(PIN_PUMP_PWM, GPIO_AF_PWM);	
		rcc_tim(benzo->pump.pwm.tim_number, ENABLE);
		pwm_setup(benzo);

		//enable reading pump current
		benzoAdc_init(ADCx, adc_channel, benzo);
		
		//reset all errors
		benzo->errors.current_sensor_error = RESET;	
		benzo->errors.decompression = RESET;
		benzo->errors.overload_current = RESET;
		benzo->errors.pressure_sensor_error = RESET;
		benzo->errors.rotation_sensor_error = RESET;	
}

//====== setup pwm output (for control pump speed) 
static void pwm_setup(benzo_t* const benzo)
{	
		TIM_TimeBaseInitTypeDef TIM_SETUP; 
		TIM_SETUP.TIM_CounterMode = TIM_CounterMode_Up;   
	
	
		TIM_SETUP.TIM_Period = PUMP_PWM_PERIOD;     
		TIM_SETUP.TIM_Prescaler = PUMP_PWM_PRESC;    
		
		TIM_TimeBaseInit(TIM1, &TIM_SETUP);
 
		TIM_OCInitTypeDef PWM_SETUP = 
		{ 
			.TIM_Pulse 				= 0x0000, 
			.TIM_OCMode 			= TIM_OCMode_PWM1,
			.TIM_OutputState 	= TIM_OutputState_Enable, 
			.TIM_OCPolarity 	= TIM_OCPolarity_High 
		};
		
		switch(benzo->pump.pwm.tim_channel)
		{
			case 1:
				TIM_OC1Init(benzo->pump.pwm.tim_number, &PWM_SETUP);
				TIM_OC1PreloadConfig(benzo->pump.pwm.tim_number, TIM_OCPreload_Enable);
			break;
	
			case 2:
				TIM_OC2Init(benzo->pump.pwm.tim_number, &PWM_SETUP);
				TIM_OC2PreloadConfig(benzo->pump.pwm.tim_number, TIM_OCPreload_Enable);
				break;
			
			case 3:
				TIM_OC3Init(benzo->pump.pwm.tim_number, &PWM_SETUP);
				TIM_OC3PreloadConfig(benzo->pump.pwm.tim_number, TIM_OCPreload_Enable);
				break;
			
			case 4:
				TIM_OC4Init(benzo->pump.pwm.tim_number, &PWM_SETUP);
				TIM_OC4PreloadConfig(benzo->pump.pwm.tim_number, TIM_OCPreload_Enable);		
				break;
			
			default:
				break;
		}
		
		TIM_ARRPreloadConfig(benzo->pump.pwm.tim_number, ENABLE);	
		TIM_Cmd(benzo->pump.pwm.tim_number, ENABLE);
		
		TIM_BDTRInitTypeDef bdtr;
		TIM_BDTRStructInit(&bdtr);
		bdtr.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
		TIM_BDTRConfig(benzo->pump.pwm.tim_number, &bdtr);		
}

//=====ADC setup (pump current)==== 
static void benzoAdc_init(ADC_TypeDef* ADCx, uint8_t channel, benzo_t* const benzo)
{
		pin_init(PIN_PUMP_ADC);
		benzo->current.adc_val_adr = ADC_InjectedAddToRotation (ADCx, channel, ADC_SampleTime_480Cycles);	
}

//=============================================================================
static ErrorStatus check_errors(benzo_t* const benzo)
{
		uint8_t error_status = 0;
	
		//check motor current for overload
		if(benzo->current.value_a > MAX_CURRENT)
		{
			benzo->errors.overload_current = SET; 
			error_status++;
		}
		else
		{
			benzo->errors.overload_current = RESET;
		}
		
		//check decompression of system
		if((benzo->current.value_a > MIN_CURRENT) && 
			(benzo->pressure.value_atm < MIN_PRESSURE))
		{
			benzo->errors.decompression = SET;
			error_status++;
		}
		else
		{
			benzo->errors.decompression = RESET;
		}	
		
		//pressure sensor error
		if(benzo->pressure.sensor.ms4525.fl_error == SET)
		{
			benzo->pressure.old_value_atm = benzo->pressure.value_atm; //remember old value
			benzo->errors.pressure_sensor_timeout.stop = 0;  //start timer
			
			if(software_timer(&benzo->errors.pressure_sensor_timeout))  //if timeout is reached
			{
				benzo->errors.pressure_sensor_error = SET;
			}
		}
		else
		{
			benzo->errors.pressure_sensor_error = RESET;
			benzo->errors.pressure_sensor_timeout.stop = 1;
		}
		
		//current sensor error		
		if(benzo->current.value_a < MIN_CURRENT)
		{
			benzo->errors.current_sensor_error = SET;
		}			
		else
		{
			benzo->errors.current_sensor_error = RESET;
		}
		
		//check rotation sensor
		if(1)
		{
			benzo->errors.rotation_sensor_error = RESET;
		}
		
		if(error_status != 0)
		{
			return ERROR;
		}
		else
		{
			return SUCCESS;
		}
			
}

//=============================================================================
void pressure_proc(int32_t * val, ms5425do_t * presure, uint8_t * event)
{
	if (*event)
	{
		if (ms4525do_start_getting(presure))
		{
			*event = RESET;
		}
	}
	
	ms4525do_task(presure);
	
	if (presure->fl_error)
	{
		presure->fl_error = RESET;
	}
	
	if (presure->fl_update)
	{
		presure->fl_update = RESET;
		
		*val = presure->value_mpsi;
	}
}
