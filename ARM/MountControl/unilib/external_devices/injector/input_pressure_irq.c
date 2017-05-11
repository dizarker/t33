//Орлан. ЭБУ. Впускной коллектор.
//Журавлев ЕА

#include  "include.h"

//================================================================
#define TIM_MAX_CNT 0x0271 //625 us = 1600 Hz
#define TIM TIM4
#define TIM_F_HZ 1000000 //1 cnt = 1 us

//================================================================
input_pressure_t* input_pressure_pt;

//================================================================
void input_pressure_irq_init(input_pressure_t* input_pressure)
{		
	//SAVE POINTER
	input_pressure_pt = input_pressure;
	
	//ENABLE RCC
	rcc_tim(TIM, ENABLE);
	
	//GET TIM FREQ
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	//BASE INIT
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period 				= TIM_MAX_CNT;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
  
	#if F1_CHECK
		uint32_t pclk_prescaler = 1;
	#else
		uint32_t pclk_prescaler = 2;
	#endif
	
	if (TIM == TIM1 ||
			TIM == TIM8 ||
			TIM == TIM9 ||
			TIM == TIM10 ||
			TIM == TIM11)
	{
		TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK2_Frequency * pclk_prescaler) / TIM_F_HZ - 1;
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.PCLK1_Frequency * pclk_prescaler) / TIM_F_HZ - 1;
	}
	
	TIM_TimeBaseInit(TIM, &TIM_TimeBaseStructure);
		
	TIM_ITConfig(TIM, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM, ENABLE);
	
	NVIC_EnableIRQ(TIM4_IRQn);
}

//================================================================
void TIM4_IRQHandler(void)
{
	ENTER_CRITICAL_SECTION();	
	
	if (TIM_GetITStatus(TIM, TIM_IT_Update) != RESET)
	{		
		TIM_ClearITPendingBit(TIM, TIM_IT_Update);
		
		input_pressure_get_value_irq(input_pressure_pt);
	}
	
	LEAVE_CRITICAL_SECTION();
}
