#ifndef _ADC_BASE_H_
#define _ADC_BASE_H_


#define ADC_RANGE	4096
#define USUPPLY		3.3F

#define ADC_STEP	(USUPPLY/ADC_RANGE)


typedef struct
{
	uint32_t *cnt;
	float 	volt;		
}adc_sense_t;

float adc_raw_transform_to_volt(uint32_t adc_raw);
float voltage_divider_inverse_transform(float Uout, float R_hight_shoulder, float R_low_shoulder);
float voltage_divider_forward_transform(float Uin, float R_hight_shoulder, float R_low_shoulder);
float acs713_volt_to_current(float Uin);


#endif
