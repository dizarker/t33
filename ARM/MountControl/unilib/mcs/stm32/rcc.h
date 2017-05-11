//Модуль для простого включения тактирования
//Журавлев

//================================================================
#if F1_CHECK //F1

ErrorStatus rcc_tim(TIM_TypeDef* TIMx, FunctionalState NewState);
ErrorStatus rcc_adc(ADC_TypeDef* ADCx, FunctionalState NewState);
ErrorStatus rcc_gpio(GPIO_TypeDef* GPIOx, FunctionalState NewState);

#else //F4

ErrorStatus rcc_dma(DMA_TypeDef* DMAx, FunctionalState NewState);
ErrorStatus rcc_tim(TIM_TypeDef* TIMx, FunctionalState NewState);
ErrorStatus rcc_adc(ADC_TypeDef* ADCx, FunctionalState NewState);
ErrorStatus rcc_gpio(GPIO_TypeDef* GPIOx, FunctionalState NewState);

#endif
