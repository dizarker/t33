#include "include.h"

//================================================================
void uni_tim_base_init(TIM_TypeDef* TIMx, uint32_t period_cnt, uint32_t tim_frq_hz)
{
	//ENABLE RCC
	rcc_tim(TIMx, ENABLE);
		
	//GET TIM FREQ
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	//BASE INIT
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period 				= period_cnt;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
  
	#if F1_CHECK
		uint32_t pclk_prescaler = 1;
	#else
		uint32_t pclk_prescaler = 2;
	#endif
	
	if (TIMx == TIM1 ||
			TIMx == TIM8 ||
			TIMx == TIM9 ||
			TIMx == TIM10 ||
			TIMx == TIM11)
	{
		TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK2_Frequency * pclk_prescaler) / tim_frq_hz - 1;
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK1_Frequency * pclk_prescaler) / tim_frq_hz - 1;
	}
	
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	
	TIM_Cmd(TIMx, ENABLE);
}

//================================================================
#if F1_CHECK
	void uni_tim_pwm_init(TIM_TypeDef* TIMx, volatile uint16_t * CCRx, uint32_t Pulse, uint16_t TIM_OCPolarity, uint8_t EnableOutput_fl)
#else
	void uni_tim_pwm_init(TIM_TypeDef* TIMx, volatile uint32_t * CCRx, uint32_t Pulse, uint16_t TIM_OCPolarity, uint8_t EnableOutput_fl)
#endif
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_OCStructInit(&TIM_OCInitStructure);
		
  TIM_OCInitStructure.TIM_OCMode 			= TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse 			= Pulse;
	TIM_OCInitStructure.TIM_OCPolarity 	= TIM_OCPolarity;
  
	if (CCRx == &(TIMx->CCR1))
	{
		TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
		TIM_OC1Init(TIMx, &TIM_OCInitStructure);
	}
	else if (CCRx == &(TIMx->CCR2))
	{
		TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
		TIM_OC2Init(TIMx, &TIM_OCInitStructure);
	}
	else if (CCRx == &(TIMx->CCR3))
	{
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
		TIM_OC3Init(TIMx, &TIM_OCInitStructure);
	}
	else if (CCRx == &(TIMx->CCR4))
	{
		TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
		TIM_OC4Init(TIMx, &TIM_OCInitStructure);
	}
	
	if (EnableOutput_fl)
	{
		TIM_CtrlPWMOutputs(TIMx, ENABLE);
	}
}

//================================================================
#if F1_CHECK
	void uni_tim_ic_init(TIM_TypeDef* TIMx, volatile uint16_t * CCRx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection)
#else
	void uni_tim_ic_init(TIM_TypeDef* TIMx, volatile uint32_t * CCRx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection)
#endif
{
  TIM_ICInitTypeDef TIM_ICInitStructure;

	TIM_ICStructInit(&TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_ICFilter = 0; //TIM_ICFilter = 1 -> filter = 2 us
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection;
  
	if (CCRx == &(TIMx->CCR1))
	{
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	}
	else if (CCRx == &(TIMx->CCR2))
	{
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	}
	else if (CCRx == &(TIMx->CCR3))
	{
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	}
	else if (CCRx == &(TIMx->CCR4))
	{
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	}
	
	TIM_ICInit(TIMx, &TIM_ICInitStructure);
}
