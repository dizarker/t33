#include "adclib.h"

#if defined (STM32F10X_LD) || (STM32F10X_LD_VL) || (STM32F10X_MD) || (STM32F10X_MD_VL) || (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)

uint32_t* ADC_InjectedAddToRotation (ADC_TypeDef* ADCx, uint8_t channel, uint8_t convClocks)
{
	uint8_t chNumCur;
	uint32_t *result;
	uint8_t isFirst = 0;
	uint32_t temp;
	
	// Считаем, сколько уже каналов используется
	chNumCur = (ADCx->JSQR & ADC_JSQR_JL) >> 20;

	// Если каналов уже 4, то возвращаем 0 и ничего больше не делаем
	if (chNumCur == INJECTED_CHANNEL_MAX) return 0;
	
	if (!(ADCx->CR2 & ADC_CR2_ADON))
	{
		isFirst = 1;
		ADC_ResetCalibration(ADCx);
		while (ADC_GetResetCalibrationStatus(ADCx));
		ADC_StartCalibration(ADCx);
		while (ADC_GetCalibrationStatus(ADCx)); 
		
	}
	
	
	// Если мы уже не первые, то увеличиваем номер канала
	if (!isFirst)	chNumCur++;
	
	// Добавляем канал в ротацию
	temp = ADCx->JSQR;
	temp &=~ ADC_JSQR_JL;
	temp >>= 5;
	temp |= (channel << 15);
	temp |= chNumCur << 20;
	
	ADCx->JSQR = temp;
	
	
	
	ADC_InitTypeDef           ADC_InitStructure;
		
	//rcc_adc(ADCx, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  
	
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = (chNumCur + 1);
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 RegularChannelConfig Test */ 
	ADC_InjectedChannelConfig(ADCx, channel, (chNumCur + 1), ADC_SampleTime_55Cycles5);
	
	ADC_AutoInjectedConvCmd(ADCx, ENABLE);

	ADC_Cmd(ADCx, ENABLE);
	
	/* Start ADC1 conversion */ 
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);	
	ADC_SoftwareStartInjectedConvCmd(ADCx, ENABLE);
	
	result = (uint32_t *)&ADCx->JDR1 + chNumCur;
	
	return result;
}


void ADC_DMAChannelInit (ADC_TypeDef* ADCx, uint16_t *pointer, uint8_t chNum, ...)
{
	uint8_t *st = &chNum;
	uint32_t *p = (uint32_t*)(st)+1;
	uint8_t ch;
	
	//!!!!!!!!!!!you can use only DMA1, ADC1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
	
	DMA_InitTypeDef dma_setup;
	
	dma_setup.DMA_BufferSize = chNum;									//data size
	dma_setup.DMA_DIR = DMA_DIR_PeripheralSRC;						//periph to memory
	dma_setup.DMA_M2M = DMA_M2M_Disable; 							//memory to memory
	dma_setup.DMA_MemoryBaseAddr = (uint32_t)pointer;
	dma_setup.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;		//
	dma_setup.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_setup.DMA_Mode = DMA_Mode_Circular;
	dma_setup.DMA_PeripheralBaseAddr = (uint32_t)&ADCx->DR;
	dma_setup.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma_setup.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_setup.DMA_Priority = DMA_Priority_Medium;
	
	DMA_Init(DMA1_Channel1, &dma_setup);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	ADC_InitTypeDef           ADC_InitStructure;
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = chNum;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 RegularChannelConfig Test */ 
	//((uint8_t)0x00)
	uint8_t n = 1;	
		
	while ((n <= chNum) && (n < 16))
	{
		if (p != 0)
		{
			ch = *p;
									
			ADC_RegularChannelConfig(ADCx, (uint8_t)ch, n, ADC_SampleTime_239Cycles5);
			
			n++;
			p++;
		}
	}
	
    ADC_DMACmd(ADCx, ENABLE);
  
	ADC_Cmd(ADCx, ENABLE);
	
	/* Start ADC1 conversion */ 
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
	
}

#elif defined (STM32F40_41xxx) || (STM32F427_437xx) || (STM32F429_439xx) || (STM32F401xx) || (STM32F411xE)

/* Функция добавляет указанные входы АЦП в регулярные каналы и запускает конвертацию						*/
/* Используется DMA для сохранения данных в памяти 																							*/
/* Перед вызовов функции необходимо включить тактование порта и настроить пины									*/
/* ADC_TypeDef *ADCx - Номер используемого ADC																									*/
/* uint16_t *pointer - указатель на начало области памяти, для сохранения результатов						*/
/* uint8_t chNum - количесвто используемых каналов (максимум 16)																*/
/* ...  1) номер канала в виде обычного числа (0 - канал 0, 1 - канал 1, 13 - канал 13)					*/
void ADC_DMAChannelInit (ADC_TypeDef* ADCx, uint16_t *pointer, uint8_t chNum, ...)
{
	uint8_t n = 0;
	uint8_t *st = &chNum;
	uint32_t *p = (uint32_t*)(st)+1;
	uint8_t ch;
	DMA_Stream_TypeDef *DMAx;
	uint32_t DMA_Channel;
	
	DMA_InitTypeDef sDMA;
	
	// Подаем такты на АЦП и DMA
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC, ENABLE);
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_DMA2, ENABLE);
	
	// Включаем режим сканирования и повторения
	ADCx->CR1 |= ADC_CR1_SCAN;
	ADCx->CR2 |= ADC_CR2_CONT;

	while ((n < chNum) && (n < 16))
	{
		if (p != 0)
		{
			ch = *p;
									
			if (n < 6) ADCx->SQR3 |= (ch << (5 * n));
			else if (n < 12) ADCx->SQR2 |= (ch << (5 * (n - 6)));
			else ADCx->SQR1 |= (ch << (5 * (n - 12)));
			
			if (ch < 10) ADCx->SMPR2 |= (7 << (3 * ch ));
			else ADCx->SMPR1 |= (7 << (3 * (ch - 10)));
			
			n++;
			p++;
		}
		else break;
	}
	ADCx->SQR1 |= (n-1) << 20;
	
	// Теперь конфигурируем DMA
	if (ADCx == ADC1) 
	{
		DMAx = DMA2_Stream0;
		DMA_Channel = DMA_Channel_0;
	}
	else if (ADCx == ADC2)
	{
		DMAx = DMA2_Stream2;
		DMA_Channel = DMA_Channel_1;
	}
	else
	{
		DMAx = DMA2_Stream1;
		DMA_Channel = DMA_Channel_2;
	}
	
	sDMA.DMA_BufferSize = n;
	sDMA.DMA_Channel = DMA_Channel;
	sDMA.DMA_DIR = DMA_DIR_PeripheralToMemory;
	sDMA.DMA_FIFOMode = DMA_FIFOMode_Disable;
	sDMA.DMA_Memory0BaseAddr = (uint32_t)pointer;
	sDMA.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	sDMA.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	sDMA.DMA_MemoryInc = DMA_MemoryInc_Enable;
	sDMA.DMA_Mode = DMA_Mode_Circular;
	sDMA.DMA_PeripheralBaseAddr = (uint32_t)&ADCx->DR;
	sDMA.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	sDMA.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	sDMA.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	sDMA.DMA_Priority = DMA_Priority_Medium;
	
	DMA_Init (DMAx, &sDMA);
	DMA_Cmd (DMAx, ENABLE);
	
	ADCx->CR2 |= ADC_CR2_DDS;
	ADCx->CR2 |= ADC_CR2_DMA;	
	ADCx->CR2 |= ADC_CR2_ADON;
	ADCx->CR2 |= ADC_CR2_SWSTART;
}


/* Функция добавляет канал в ротацию инжектированных каналов																*/
/* И возвращает указатель на регистр, откуда можно будет забирать данные										*/
/* Входные аргументы:																																				*/
/* ADC_TypeDef *ADCx - номер ADC (ADC1, ADC2, ADC3)																					*/
/* uint8_t channel - номер канала ADC (0, 1, 2, ..., 18)																		*/
/* uint8_t convClocks - количество циклов АЦП на преобразование (ADC_SampleTime_480Cycles..)*/
/* Возвращаемое значение: указатель на регистр, в котором находится результат								*/
uint32_t* ADC_InjectedAddToRotation (ADC_TypeDef* ADCx, uint8_t channel, uint8_t convClocks)
{
	uint8_t chNumCur;
	uint8_t isFirst = 0;
	uint32_t *result;
	uint32_t temp;
	
	rcc_adc(ADCx, ENABLE);
	
	ADC_CommonInitTypeDef sADC;
	sADC.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	sADC.ADC_Mode = ADC_Mode_Independent;
	sADC.ADC_Prescaler = ADC_Prescaler_Div8;
	sADC.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit (&sADC);
	
	// Считаем, сколько уже каналов используется
	chNumCur = (ADCx->JSQR & ADC_JSQR_JL) >> 20;
	
	// Если каналов уже 4, то возвращаем 0 и ничего больше не делаем
	if (chNumCur == INJECTED_CHANNEL_MAX) return 0;
	
	// Если бит ADON не стоит, то значит АЦП еще не используется и значит этот канал будет первым
	if (!(ADCx->CR2 & ADC_CR2_ADON)) isFirst = 1;
	
	// Выключаем АЦП
	ADCx->CR2 &=~ ADC_CR2_ADON;
	
	// Если мы уже не первые, то увеличиваем номер канала
	if (!isFirst)	chNumCur++;
	
	// Добавляем канал в ротацию
	temp = ADCx->JSQR;
	temp &=~ ADC_JSQR_JL;
	temp >>= 5;
	temp |= (channel << 15);
	temp |= chNumCur << 20;
	
	ADCx->JSQR = temp;
	
	// Выставляем время преобразования на указанный канал
	if (channel < 10) ADCx->SMPR2 |= (convClocks << (3 * channel));
	else ADCx->SMPR1 |= (convClocks << (3 * (channel - 10)));
	
	// Если это первый вызов - то конфигурируем АЦП в режим непрерывного преобразования в ротации инжектированных каналов
	if (isFirst)
	{
		ADCx->CR1 |= ADC_CR1_SCAN | ADC_CR1_JAUTO;
		ADCx->CR2 |= ADC_CR2_CONT;
	}
	
	// Включаем АЦП и стартуем
	ADCx->CR2 |= ADC_CR2_ADON;
	ADCx->CR2 |= ADC_CR2_JSWSTART | ADC_CR2_SWSTART;
	
	// Получим адрес регистра, в котором у нас будет результат
	result = (uint32_t*)&(ADCx->JDR1) + chNumCur;
	
	return result;
}

#else 
#error "This mcu is not supported, or select mcu type"

#endif

