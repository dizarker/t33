#include "include.h"


//=============================================================================
float adc_raw_transform_to_volt(uint32_t adc_raw)
{		

	return (float)adc_raw*ADC_STEP;
}

//=============================================================================
float voltage_divider_inverse_transform(float Uout, float R_hight_shoulder, float R_low_shoulder)
{	
	return ((Uout*(R_hight_shoulder + R_low_shoulder))/R_low_shoulder);	
}

//=============================================================================
float acs713_volt_to_current(float Uin)
{	
	return (Uin-0.5)*7.5;
}
