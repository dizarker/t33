//Орлан. ЭБУ. Зажигание.
//Журавлев ЕА

#include  "include.h"

//================================================================
void ignition_pwr_on(void)
{
	pin_set(IGNITION_PIN_PWR);	
}

//================================================================
void ignition_pwr_off(void)
{
	pin_clr(IGNITION_PIN_PWR);	
}

//================================================================
void ignition_init(ignition_t* ignition)
{	
	pin_init(IGNITION_PIN_PWR);
	
	//ENABLE RCC
	rcc_tim(ignition->settings.TIMx, ENABLE);
		
	//GET TIM FREQ
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	//BASE INIT
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period 				= 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
  
	#if F1_CHECK
		uint32_t pclk_prescaler = 1;
	#else
		uint32_t pclk_prescaler = 2;
	#endif
	
	if (ignition->settings.TIMx == TIM1 ||
			ignition->settings.TIMx == TIM8 ||
			ignition->settings.TIMx == TIM9 ||
			ignition->settings.TIMx == TIM10 ||
			ignition->settings.TIMx == TIM11)
	{
		TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK2_Frequency * pclk_prescaler) / 1000000 - 1;
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK1_Frequency * pclk_prescaler) / 1000000 - 1;
	}
	
	TIM_TimeBaseInit(ignition->settings.TIMx, &TIM_TimeBaseStructure);
	
	//OC INIT
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
  TIM_OCInitStructure.TIM_OCMode 			= TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse 			= 1;
  if (!ignition->settings.TIM_OCPolarity)
	{
		TIM_OCInitStructure.TIM_OCPolarity 	= TIM_OCPolarity_High;
	}
	else
	{
		TIM_OCInitStructure.TIM_OCPolarity 	= ignition->settings.TIM_OCPolarity;
	}
  
	if (ignition->settings.CCRx == &(ignition->settings.TIMx->CCR1))
	{
		TIM_OC1Init(ignition->settings.TIMx, &TIM_OCInitStructure);
	}
	else if (ignition->settings.CCRx == &(ignition->settings.TIMx->CCR2))
	{
		TIM_OC2Init(ignition->settings.TIMx, &TIM_OCInitStructure);
	}
	else if (ignition->settings.CCRx == &(ignition->settings.TIMx->CCR3))
	{
		TIM_OC3Init(ignition->settings.TIMx, &TIM_OCInitStructure);
	}
	else if (ignition->settings.CCRx == &(ignition->settings.TIMx->CCR4))
	{
		TIM_OC4Init(ignition->settings.TIMx, &TIM_OCInitStructure);
	}
	
	//ONE PULSE SELECT
	TIM_SelectOnePulseMode(ignition->settings.TIMx, TIM_OPMode_Single);
	
	//ENABLE OUTPUT
	TIM_CtrlPWMOutputs(ignition->settings.TIMx, ENABLE);
}

//================================================================
void ignition_pulse(ignition_t* ignition, uint32_t delay_us)
{
	delay_us -= ignition->in.pulse_delay_after_signal_us;
	
	if (delay_us > ignition->in.pulse_signal_length_us)
	{
		ignition->settings.TIMx->ARR	= delay_us;
		*(ignition->settings.CCRx)	= delay_us - ignition->in.pulse_signal_length_us;
	}
	else
	{
		ignition->errors.set_too_small_delay = 1;
		ignition->settings.TIMx->ARR	= ignition->in.pulse_signal_length_us;
		*(ignition->settings.CCRx) = 1;
	}
	
	ignition->settings.TIMx->CR1 |= TIM_CR1_CEN;
	
	ignition->out.pulse_cnt++;
}
