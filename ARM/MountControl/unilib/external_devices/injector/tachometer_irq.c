//Орлан. ЭБУ. Впускной коллектор.
//Журавлев ЕА

#include  "include.h"

//================================================================
#define TIM_MAX_CNT 0xFFFF
#define TIM TIM3
#define TIM_F_HZ 500000 //1 cnt = 2 us

//================================================================
static tachometer_t * tachometer_pt;

//================================================================
void tachometer_irq_init(tachometer_t* tachometer)
{		
	//SAVE POINTER
	tachometer_pt = tachometer;
	
	//SET PARAM TO STRUCT
	tachometer->tim_capture.f_hz = TIM_F_HZ;
	tachometer->tim_capture.max_cnt = TIM_MAX_CNT;
	
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
	
	//IC INIT
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 10; //TIM_ICFilter = 1 -> filter = 2 us
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	
	TIM_ICInit(TIM, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	
	TIM_ICInit(TIM, &TIM_ICInitStructure);
	
	TIM_ITConfig(TIM, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM, TIM_IT_CC2, ENABLE);
	
	TIM_Cmd(TIM, ENABLE);
	
	NVIC_EnableIRQ(TIM3_IRQn);
}

//================================================================
void TIM3_IRQHandler(void)
{
	ENTER_CRITICAL_SECTION();	
	
	if (TIM_GetITStatus(TIM, TIM_IT_CC1) != RESET) //7pulse
	{		
		TIM_ClearITPendingBit(TIM, TIM_IT_CC1);
		
		tachomater_7pulse_irq(tachometer_pt, TIM_GetCapture1(TIM));
	}
	else if (TIM_GetITStatus(TIM, TIM_IT_CC2) != RESET) //1pulse
	{
		TIM_ClearITPendingBit(TIM, TIM_IT_CC2);
		tachomater_1pulse_irq(tachometer_pt, TIM_GetCapture2(TIM));
	}
	
	LEAVE_CRITICAL_SECTION();
}
