
#ifndef _LM135_H_
#define _LM135_H_

/*
//================================================================
lm135_t lm135 = {
	.hw.ADCx = ADC1,
	.hw.adc_channel = 14,
	.hw.adc_convClocks = ADC_SampleTime_480Cycles,
	.hw.signal_division = 2,
	.hw.maximum_normal_voltage_mv = 2000,
	.filter.length = 50,
	.timeout_update.ms = 20, 
};
*/

//================================================================
typedef struct
{
	struct
	{
		ADC_TypeDef*	ADCx;
		uint8_t 			adc_channel;
		uint8_t 			adc_convClocks;
		uint8_t				signal_division;
		uint16_t			maximum_normal_voltage_mv;
	} hw;
	
	filter_hi_pass_fast_start_t filter;
	timeout_t timeout_update;
	
	uint16_t* adc_cnt;
	
	uint8_t not_work_fl;
	float val_c;
	
}lm135_t;

//================================================================
void lm135_init(lm135_t * sensor);
void lm135_task(lm135_t * sensor);

#endif
