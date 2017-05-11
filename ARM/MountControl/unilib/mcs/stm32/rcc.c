//Модуль для простого включения тактирования
//Журавлев
#include  "include.h"

#if F1_CHECK //F1

ErrorStatus rcc_tim(TIM_TypeDef* TIMx, FunctionalState NewState)
{ 
  if (TIMx == TIM1)
  {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, NewState);
  } 
  else if (TIMx == TIM2) 
  {     
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, NewState);
  }  
  else if (TIMx == TIM3)
  { 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, NewState);
  }  
  else if (TIMx == TIM4)
  { 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, NewState);
  }  
  else if (TIMx == TIM5)
  {      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, NewState);
  }  
  else if (TIMx == TIM6)  
  {    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, NewState);
  }  
  else if (TIMx == TIM7)
  {      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, NewState);
  }  
  else if (TIMx == TIM8)
  {      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, NewState);
  }  
  else if (TIMx == TIM9)
  {      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, NewState);  
   }  
  else if (TIMx == TIM10)
  {      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, NewState);
  }  
  else if (TIMx == TIM11) 
  {     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, NewState);
  }  
  else if (TIMx == TIM12)
  {      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, NewState); 
  }  
  else if (TIMx == TIM13) 
  {       
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, NewState); 
  }  
  else if (TIMx == TIM14) 
  {     
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, NewState);
  }
	else
	{
		return ERROR;
	}
	return SUCCESS;
}

//ADC
//================================================================
ErrorStatus rcc_adc(ADC_TypeDef* ADCx, FunctionalState NewState)
{
	switch ((uint32_t) ADCx)
	{
		case (uint32_t)ADC1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, NewState);
			return SUCCESS;
		case (uint32_t)ADC2:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, NewState);
			return SUCCESS;
		case (uint32_t)ADC3:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, NewState);
			return SUCCESS;
		default:
			return ERROR;
	}
}

//GPIO
//================================================================
ErrorStatus rcc_gpio(GPIO_TypeDef* GPIOx, FunctionalState NewState)
{
  if (GPIOx == GPIOA)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOB)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOC)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOD)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOE)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOF)
  {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOG)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, NewState);
		return SUCCESS;
  }
	return ERROR;
}

#else //F4

//DMA
//================================================================
ErrorStatus rcc_dma(DMA_TypeDef* DMAx, FunctionalState NewState)
{ 
  if (DMAx == DMA1)
  {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, NewState);
  } 
  else if (DMAx == DMA2) 
  {     
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, NewState);
  }  
	else
	{
		return ERROR;
	}
	return SUCCESS;
}

//TIM
//================================================================
ErrorStatus rcc_tim(TIM_TypeDef* TIMx, FunctionalState NewState)
{ 
  if (TIMx == TIM1)
  {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, NewState);
  } 
  else if (TIMx == TIM2) 
  {     
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, NewState);
  }  
  else if (TIMx == TIM3)
  { 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, NewState);
  }  
  else if (TIMx == TIM4)
  { 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, NewState);
  }  
  else if (TIMx == TIM5)
  {      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, NewState);
  }  
  else if (TIMx == TIM6)  
  {    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, NewState);
  }  
  else if (TIMx == TIM7)
  {      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, NewState);
  }  
  else if (TIMx == TIM8)
  {      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, NewState);
  }  
  else if (TIMx == TIM9)
  {      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, NewState);  
   }  
  else if (TIMx == TIM10)
  {      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, NewState);
  }  
  else if (TIMx == TIM11) 
  {     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, NewState);
  }  
  else if (TIMx == TIM12)
  {      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, NewState); 
  }  
  else if (TIMx == TIM13) 
  {       
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, NewState); 
  }  
  else if (TIMx == TIM14) 
  {     
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, NewState);
  }
	else
	{
		return ERROR;
	}
	return SUCCESS;
}

//ADC
//================================================================
ErrorStatus rcc_adc(ADC_TypeDef* ADCx, FunctionalState NewState)
{
	switch ((uint32_t) ADCx)
	{
		case (uint32_t)ADC:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, NewState);
			return SUCCESS;
		case (uint32_t)ADC1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, NewState);
			return SUCCESS;
		case (uint32_t)ADC2:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, NewState);
			return SUCCESS;
		case (uint32_t)ADC3:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, NewState);
			return SUCCESS;
		default:
			return ERROR;
	}
}

//GPIO
//================================================================
ErrorStatus rcc_gpio(GPIO_TypeDef* GPIOx, FunctionalState NewState)
{
  if (GPIOx == GPIOA)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOB)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOC)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOD)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOE)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOF)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOG)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOH)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOI)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOJ)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOJ, NewState);
		return SUCCESS;
  }
  else if (GPIOx == GPIOK)
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOK, NewState);
		return SUCCESS;
  }
	return ERROR;
}

#endif
