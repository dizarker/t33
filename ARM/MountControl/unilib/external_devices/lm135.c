#include  "include.h"

#define LM135_MV_ZERO_C 2730
#define LM135_MV_RAT_C 10
#define LM135_C(mv) ((((float) mv) - LM135_MV_ZERO_C) / LM135_MV_RAT_C)

//================================================================
void lm135_init(lm135_t * sensor)
{
	//sensor->adc_cnt = ADC_InjectedAddToRotation(sensor->hw.ADCx, sensor->hw.adc_channel, sensor->hw.adc_convClocks);
}

//================================================================
void lm135_task(lm135_t * sensor)
{
	uint16_t mv;
	
	filter_hi_pass_fast_start(&sensor->filter, *(sensor->adc_cnt));
	
	mv = ADC_TO_MV(sensor->filter.data_out);
	
	if (mv > sensor->hw.maximum_normal_voltage_mv)
	{
		sensor->not_work_fl = 1;
	}
	else
	{
		sensor->not_work_fl = 0;
	}
	
	sensor->val_c = LM135_C( sensor->hw.signal_division * mv);
}
