// Servo-motors pwm control

/*
*	WARNING! IF USE HSE, NEED TRUE DEFINE "HSE_VALUE"
*	EXAMPLE: HSE_VALUE=16000000
*/

//need: rcc.h, stm32f4xx_tim.h, stm32f4xx.h
#include "include.h"

#if defined (STM32F40_41xxx) || (STM32F427_437xx) || (STM32F429_439xx) || (STM32F401xx) || (STM32F411xE)
#else
#warning Module tested only on stmf4xx!
#endif

//================================================================
static ErrorStatus set_to_tim_ccr(TIM_TypeDef* tim, uint8_t tim_ch, uint32_t value)
{
	switch(tim_ch)
	{
	case 1:
		tim->CCR1 = value;
		break;
	
	case 2:
		tim->CCR2 = value;
		break;
	
	case 3:
		tim->CCR3 = value;
		break;
	
	case 4:
		tim->CCR4 = value;
		break;
	
	default:
		return ERROR;
	}
	return SUCCESS;
}

//================================================================
uint32_t servo_get_pos_us_orientation(servo_t* servo)
{
	if (servo->orientation == SERVO_ORIENTATION_NORMAL)
	{
		return servo->position_us;
	}
	else
	{
		return servo->position_max_us - (servo->position_us - servo->position_min_us);
	}
}

//================================================================
ErrorStatus servo_init (servo_t* servo, TIM_TypeDef* tim, uint8_t tim_ch)
{
	//----------------------------------------------
	servo->tim 		= tim;
	servo->tim_ch = tim_ch;
	
	servo->period_us 				= SERVO_PERIOD_US_DEFAULT;
	servo->position_min_us	= SERVO_POSITION_MIN_US_DEFAULT;
	servo->position_max_us	= SERVO_POSITION_MAX_US_DEFAULT;
	servo->orientation			= SERVO_ORIENTATION_DEFAULT;
	
	servo->position 				= SERVO_POSITION_DEFAULT;
	servo->position_us 			= SERVO_POSITION_US_DEFAULT;
	
	servo->enable			 			= RESET;
	
	//----------------------------------------------
	if (rcc_tim(servo->tim, ENABLE) == ERROR)
	{
		return ERROR;
	}
	
	//----------------------------------------------
	RCC_ClocksTypeDef RCC_Clocks;
	
	RCC_GetClocksFreq(&RCC_Clocks);
	
	//----------------------------------------------
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	if (servo->tim == TIM1 ||
			servo->tim == TIM8 ||
			servo->tim == TIM9 ||
			servo->tim == TIM10 ||
			servo->tim == TIM11)
	{
		TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK2_Frequency * 2) / 1000000 - 1;
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK1_Frequency * 2) / 1000000 - 1;
	}
	
	TIM_TimeBaseStructure.TIM_Period = servo->period_us;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(servo->tim, &TIM_TimeBaseStructure);

	//----------------------------------------------
	TIM_ARRPreloadConfig(servo->tim, ENABLE);
	
	//----------------------------------------------
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_OCStructInit(&TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	if (servo->enable == SET)
	{
		TIM_OCInitStructure.TIM_Pulse = servo->position_us;
	}
	else
	{
		TIM_OCInitStructure.TIM_Pulse = 0;
	}
	
	//----------------------------------------------
	switch (servo->tim_ch)
	{
		case 1:
			TIM_OC1Init(servo->tim, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(servo->tim, TIM_OCPreload_Enable);
			break;
		
		case 2:
			TIM_OC2Init(servo->tim, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(servo->tim, TIM_OCPreload_Enable);
			break;
		
		case 3:
			TIM_OC3Init(servo->tim, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(servo->tim, TIM_OCPreload_Enable);
			break;
		
		case 4:
			TIM_OC4Init(servo->tim, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(servo->tim, TIM_OCPreload_Enable);
			break;
		
		default:
			return ERROR;
	}
	
	TIM_CtrlPWMOutputs(servo->tim, ENABLE);
	
	TIM_Cmd(servo->tim, ENABLE);
	
	return SUCCESS;
}

//================================================================
ErrorStatus servo_enable (servo_t* servo)
{
	servo->enable = SET;
	return (set_to_tim_ccr(servo->tim, servo->tim_ch, servo->position_us));
}

//================================================================
ErrorStatus servo_disable (servo_t* servo)
{
	servo->enable = RESET;
	return (set_to_tim_ccr(servo->tim, servo->tim_ch, 0));
}

//================================================================
ErrorStatus servo_set_pos (servo_t* servo, float position)
{
	if (servo->enable == RESET)
	{
		return ERROR;
	}
	
	if (position > SERVO_POSITION_MAX || position < SERVO_POSITION_MIN)
	{
		return ERROR;
	}
	
	uint32_t position_us;
	
	position_us = LINEAR_DEPEND_Y(position, SERVO_POSITION_MIN, SERVO_POSITION_MAX, servo->position_min_us, servo->position_max_us);
	
	if (position_us > servo->position_max_us || position_us < servo->position_min_us) return ERROR;
	
	servo->position_us = position_us;
	servo->position = position;
	
	return (set_to_tim_ccr(servo->tim, servo->tim_ch, servo_get_pos_us_orientation(servo)));
}

//================================================================
ErrorStatus servo_set_pos_us (servo_t* servo, uint32_t position_us)
{
	if (servo->enable == RESET)
	{
		return ERROR;
	}
	
	if (position_us > servo->position_max_us || position_us < servo->position_min_us) return ERROR;
	
	servo->position_us = position_us;
	servo->position = LINEAR_DEPEND_Y((float)position_us, servo->position_min_us, servo->position_max_us, SERVO_POSITION_MIN, SERVO_POSITION_MAX);
	
	return (set_to_tim_ccr(servo->tim, servo->tim_ch, servo_get_pos_us_orientation(servo)));
}

//================================================================
void servo_set_pos_us_nocheck (servo_t* servo, uint32_t position_us)
{
	servo->position_us = position_us;
	servo->position = LINEAR_DEPEND_Y((float)position_us, servo->position_min_us, servo->position_max_us, SERVO_POSITION_MIN, SERVO_POSITION_MAX);
	
	set_to_tim_ccr(servo->tim, servo->tim_ch, servo_get_pos_us_orientation(servo));
}

//================================================================
float servo_get_pos (servo_t* servo)
{
	return servo->position;
}

//================================================================
uint32_t servo_get_pos_us (servo_t* servo)
{
	return servo->position_us;
}

//================================================================
ErrorStatus servo_set_min_us(servo_t* servo, uint32_t position_us)
{
	if (position_us > servo->period_us || position_us > servo->position_max_us)
	{
		return ERROR;
	}
	
	servo->position_min_us = position_us;
	
	if (servo->position_us < position_us)
	{
		servo->position_us = position_us;
		servo_set_pos_us(servo, servo_get_pos_us_orientation(servo));
	}
	
	servo->position = LINEAR_DEPEND_Y((float)servo->position_us, servo->position_min_us, servo->position_max_us, SERVO_POSITION_MIN, SERVO_POSITION_MAX);
	
	return SUCCESS;
}

//================================================================
ErrorStatus servo_set_max_us(servo_t* servo, uint32_t position_us)
{
	if (position_us > servo->period_us || position_us < servo->position_min_us)
	{
		return ERROR;
	}
	
	servo->position_max_us = position_us;
	
	if (servo->position_us > position_us)
	{
		servo->position_us = position_us;
		servo_set_pos_us(servo, servo_get_pos_us_orientation(servo));
	}
	
	servo->position = LINEAR_DEPEND_Y((float)servo->position_us, servo->position_min_us, servo->position_max_us, SERVO_POSITION_MIN, SERVO_POSITION_MAX);
	
	return SUCCESS;
}

//================================================================
ErrorStatus servo_set_period(servo_t* servo, uint32_t period_us)
{
	if (period_us < servo->position_max_us)
	{
		return ERROR;
	}
	
	servo->period_us = period_us; 
	
	servo->tim->ARR = period_us;
	
	return SUCCESS;
}

//================================================================
void servo_set_orientation(servo_t* servo, servo_orientation_t orientation)
{
	servo->orientation = orientation;
	
	servo_set_pos_us(servo, servo_get_pos_us_orientation(servo));
}

//================================================================
void servo_reset_settings(servo_t* servo)
{
	servo->period_us 				= SERVO_PERIOD_US_DEFAULT;
	servo->position_min_us	= SERVO_POSITION_MIN_US_DEFAULT;
	servo->position_max_us	= SERVO_POSITION_MAX_US_DEFAULT;
	servo->orientation			= SERVO_ORIENTATION_DEFAULT;
	
	servo->position 				= SERVO_POSITION_DEFAULT;
	servo->position_us 			= SERVO_POSITION_US_DEFAULT;
	
	servo->enable			 			= RESET;
	
	servo_disable(servo);
}
