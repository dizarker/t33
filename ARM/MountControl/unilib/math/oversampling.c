#include "include.h"

//================================================================
void oversampling_init(oversampling_t* unit)
{
	unit->internal_cnt_max = pow(4, unit->settings_resolution_up);
	unit->internal_adc_cnt_max = (OVERSAMPLING_ADC_MAX_CNT + 1) * pow(2, unit->settings_resolution_up) - 1;
	unit->internal_adc_max_uv_div_adc_cnt_max = OVERSAMPLING_ADC_MAX_UV * 1000 / unit->internal_adc_cnt_max;
	unit->external_complete_fl = 0;
}

//================================================================
uint8_t oversampling(oversampling_t* unit, int16_t data)
{
	unit->internal_sum += data;
	unit->internal_cnt++;
		
	if (unit->internal_cnt >= unit->internal_cnt_max)
	{
		int32_t sum = unit->internal_sum;
		
		sum >>= unit->settings_resolution_up;
		
		unit->external_data_out_uv = sum * unit->internal_adc_max_uv_div_adc_cnt_max / 1000;
		
		unit->internal_cnt = 0;
		unit->internal_sum = 0;
		
		unit->external_complete_fl = 1;
		return 1;
	}
	return 0;
}

//================================================================
void oversampling_reset(oversampling_t* unit)
{
	unit->internal_sum = 0;
	unit->internal_cnt = 0;
	unit->external_complete_fl = 0;
}
