
#ifndef _BENZO_H_
#define _BENZO_H_

#define PIN_I2C3_SCL	A,8,H,ALT_OUTPUT_OPEN_DRAIN,SPEED_50MHZ
#define PIN_I2C3_SDA	C,9,H,ALT_OUTPUT_OPEN_DRAIN,SPEED_50MHZ

#define PIN_PUMP_PWM A,11,H,ALT_OUTPUT_PUSH_PULL,SPEED_50MHZ
#define PIN_PUMP_ADC B,0,H,ANALOG,SPEED_50MHZ
#define GPIO_AF_PWM GPIO_AF_TIM1

#define MAX_CURRENT 3.0f
#define MIN_CURRENT 0.2f
#define MIN_PRESSURE 0.1f

#define MAX_IMPACT 0x140

// pwm_freq = (168 000 000 / presc / per) //настройка на 2кГц
#define PUMP_PWM_PERIOD 1000
#define PUMP_PWM_PRESC 83
#define F_CPU 168000000

#define I2C_PRESSURE_BENZO i2c3

/*
typedef struct
{
	
}fg36_t;
*/

typedef struct
{	
  	struct
		{	
			float value_a;		
			
			float value_raw;
			float set_value_a;
			
			filter_hi_pass_t value_filter;  	
			
			uint32_t *adc_val_adr;
					
		}current;
		
		struct
		{
			float value_atm;
			float *set_value_atm;
			float old_value_atm;
			filter_hi_pass_t value_filter;			
			
			struct
			{
				uint8_t event_update;
				int32_t value_mpsi; 
				ms5425do_t ms4525;
				timeout_t timeout_request;				
			}sensor;
			
			timeout_t pid_timeout_request;
			
			pid_init_param_t pid_init;
			pid_t pid;
			float pid_limiter;
		}pressure;
	
		struct
		{
			struct
			{
				uint8_t tim_channel;
				TIM_TypeDef* tim_number;
			}pwm;
			
			struct
			{	
				uint16_t value_rpm;
			}rotation;
		}pump;
		
		struct
		{
			uint8_t overload_current;	 	
			uint8_t decompression;		
			uint8_t pressure_sensor_error;			
			uint8_t current_sensor_error;	
			uint8_t rotation_sensor_error;	
			timeout_t pressure_sensor_timeout;			
		}errors;


		uint16_t pid_timeout;
		float impct;

}benzo_t;


void benzo_set_state(benzo_t* const benzo, FunctionalState newState);        			
ErrorStatus benzo_task(benzo_t* const benzo);  
void benzo_init_fg36(benzo_t* const benzo); 																//set params

#endif

