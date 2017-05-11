#ifndef _ADCLIB_H
#define _ADCLIB_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"

#define INJECTED_CHANNEL_MAX								3

void ADC_DMAChannelInit (ADC_TypeDef* ADCx, uint16_t *pointer, uint8_t chNum, ...);
uint32_t* ADC_InjectedAddToRotation (ADC_TypeDef* ADCx, uint8_t channel, uint8_t convClocks);

#endif
