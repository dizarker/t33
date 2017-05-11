//Орлан. ЭБУ. Дроссельная заслонка.
//Журавлев ЕА

#include  "include.h"

#define ADC_MV(cnt) ((cnt * 3300) / 0xFFF)
#define ACS713_MA(mv_without_zero) (((mv_without_zero) * 1000) / 133)

//================================================================
//local prototypes
static float s_of_circle_circlr_intersection(float r, float delta);
static void throttle_servo_update_trottle_area_mm2(throttle_t * throttle);
static void throttle_servo_selftest_in_work(throttle_t * throttle);
static void throttle_servo_selftest_init(throttle_t * throttle);

static void throttle_servo_unlock(throttle_t * throttle);

static ErrorStatus throttle_current_sensor_check(throttle_t * throttle);
static void throttle_current_sensor_set_zero(throttle_t * throttle);

static uint32_t throttle_servo_current_get(throttle_t * throttle);

//local prototypes not used yet
//static void throttle_find_min_max_servo_position(throttle_t * throttle);
//static void throttle_auto_calibration(throttle_t * throttle);

//================================================================
//GLOBALE FUNCTIONS
//================================================================
	
//================================================================
void throttle_init(throttle_t * throttle, TIM_TypeDef* servo_tim, uint8_t servo_tim_ch, ADC_TypeDef* servo_current_adc, uint8_t servo_current_adc_num)
{
	//servo
	servo_init(&throttle->servo, servo_tim, servo_tim_ch);
	throttle->servo.position_min_us = throttle->settings.servo_position_min_us;
	throttle->servo.position_max_us = throttle->settings.servo_position_max_us;
	throttle->servo.orientation = throttle->settings.servo_orientation;
	
	//servo current sensor
	throttle->servo_current_sensor_adc_val_adr = ADC_InjectedAddToRotation(servo_current_adc, servo_current_adc_num, ADC_SampleTime_480Cycles);
	
	servo_disable(&throttle->servo);
	
	delay_ms(5);
	
	if (throttle_current_sensor_check(throttle) == ERROR)
	{
		throttle->errors.init_servo_current_sensor = SET;
	}
	else
	{
		throttle_current_sensor_set_zero(throttle);
		
		throttle_servo_selftest_init(throttle);
	
		if (throttle->errors.init_jammed == SET || throttle->errors.init_lock == SET)
		{
			throttle_servo_unlock(throttle);
			throttle_servo_selftest_init(throttle);
		}
	}
	
	throttle->servo_current_work_filter.length = 30;
		
	throttle->servo_selftest_timeout.end_sense 				= 0;
	throttle->servo_selftest_timeout.ms								= 5;
	throttle->servo_selftest_timeout.start_sense			= 0;
	throttle->servo_selftest_timeout.stop							=	0;
	throttle->servo_selftest_timeout.update_end_sense	=	1;
	
	servo_enable(&throttle->servo);
	servo_set_pos(&throttle->servo, 0);
}

//================================================================
void throttle_task(throttle_t * throttle)
{
	if (software_timer(&(throttle->servo_selftest_timeout)))
	{
		throttle_servo_selftest_in_work(throttle);
	}
}

//================================================================
void throttle_set_accel(throttle_t * throttle, uint8_t accel)
{
	uint32_t servo_position_us;
	
	servo_position_us = LINEAR_DEPEND_Y((uint32_t)accel, 0, 100, throttle->servo.position_min_us, throttle->servo.position_max_us);
	
	servo_set_pos_us(&throttle->servo, servo_position_us);
	
	throttle->servo_position_us = servo_position_us;
	
	throttle_servo_update_trottle_area_mm2(throttle);
}

//================================================================
void throttle_set_servo_position_us(throttle_t * throttle, uint32_t servo_position_us)
{
	servo_set_pos_us(&throttle->servo, servo_position_us);
	
	throttle->servo_position_us = servo_position_us;
	
	throttle_servo_update_trottle_area_mm2(throttle);
}

//================================================================
void throttle_set_servo_max_position_us(throttle_t * throttle, uint32_t position_us)
{
	throttle->settings.servo_position_min_us = position_us;
	servo_set_min_us(&throttle->servo, position_us);
}

//================================================================
void throttle_set_servo_min_position_us(throttle_t * throttle, uint32_t position_us)
{
	throttle->settings.servo_position_max_us = position_us;
	servo_set_max_us(&throttle->servo, position_us);
}


//================================================================
//MODULE FUNCTIONS
//================================================================

//#define S_OF_CIRCLE_CIRCLE_INTERSECTION(r, delta) (2 * r * r * acos(delta / (2 * r)) - 0.5f * delta * sqrt(4 * r * r - delta * delta))
//================================================================
static float s_of_circle_circlr_intersection(float r, float delta)
{
	float part1, part2, part3, part4, ret;
	
	part1 = 2 * r * r;
	part2 = acos(delta / (2 * r));
	part3 = 0.5f  * delta;
	part4 = sqrt(4 * r * r - delta * delta);
	
	ret = part1 * part2 - part3 * part4;
	
	return ret;
}

//================================================================
static void throttle_servo_update_trottle_area_mm2(throttle_t * throttle)
{
	float throttle_delta;
	
	throttle_delta = LINEAR_DEPEND_Y(throttle->servo_position_us, throttle->settings.servo_position_min_us, throttle->settings.servo_position_max_us, throttle->settings.throttle_diametr_mm, 0);
	throttle->throttle_area_mm2 = s_of_circle_circlr_intersection(throttle->settings.throttle_diametr_mm/2, throttle_delta);
}

//================================================================
static void throttle_servo_selftest_in_work(throttle_t * throttle)
{	
	if (throttle_current_sensor_check(throttle) == ERROR)
	{
		throttle->errors.servo_current_sensor = SET;
	}
	else
	{
		uint32_t servo_current;
		
		servo_current = throttle_servo_current_get(throttle);
		filter_arithmetic_mean(&throttle->servo_current_work_filter, servo_current);
		
		if (throttle->servo_current_work_filter.data_out > throttle->settings.servo_jammed_current_ma)
		{
			throttle->errors.lock = SET;
			throttle->errors.jammed = RESET;
		}
		else if (throttle->servo_current_work_filter.data_out > throttle->settings.servo_normal_current_ma  && !throttle->errors.lock)
		{
			throttle->errors.jammed = SET;
		}
	}
}

//================================================================
//Тест на заклинивание/заедание саервопривода при инциализации
static void throttle_servo_selftest_init(throttle_t * throttle)
{
	int32_t i, servo_current_ma;	
	filter_hi_pass_t servo_current_filter;
	
	servo_current_filter.length = 30;
	servo_current_filter.data_out = 0;
	servo_current_filter.data_sum = 0;
	
	throttle->errors.init_jammed = RESET;
	throttle->errors.init_lock = RESET;
	
	servo_enable(&throttle->servo);
	
	servo_set_pos_us(&throttle->servo, throttle->servo.position_min_us);
	delay_ms(1500);
	
	servo_set_pos_us(&throttle->servo, throttle->servo.position_min_us); //TODO: разобраться, почему без этого первый раз считывается завышенный ток
	servo_current_ma = throttle_servo_current_get(throttle);
	servo_current_filter.data_sum = servo_current_filter.length * servo_current_ma;
	
	for (i = throttle->servo.position_min_us; i < throttle->servo.position_max_us; i++)
	{
		servo_set_pos_us(&throttle->servo, i);
		
		delay_us(200);
		servo_current_ma = throttle_servo_current_get(throttle);
		delay_us(900);
		
		filter_hi_pass(&servo_current_filter, servo_current_ma);
		
		if (servo_current_filter.data_out > throttle->settings.servo_jammed_current_ma)
		{
			throttle->errors.init_lock = SET;
			throttle->errors.init_jammed = RESET;
		}
		else if (servo_current_filter.data_out > throttle->settings.servo_normal_current_ma && !throttle->errors.init_lock)
		{
			throttle->errors.init_jammed = SET;
		}
		
		/*DBG*/ //test_throttle_send_to_pc(i, servo_current_ma, servo_current_filter.data_out);
	}
	
	servo_disable(&throttle->servo);
}

#define THROTTLE_SERVO_UNLOCK_TRY_CNT 3
#define THROTTLE_SERVO_UNLOCK_DELAY_MAX_MS 600
#define THROTTLE_SERVO_UNLOCK_DELAY_MIN_MS 1
#define THROTTLE_SERVO_UNLOCK_DELAY_STEP_MS 50
//================================================================
//попытка разблокировать заклиненный/заедающий сервопривод
static void throttle_servo_unlock(throttle_t * throttle)
{
	servo_enable(&throttle->servo);
	
	int32_t i, j;
	
	for (i = THROTTLE_SERVO_UNLOCK_DELAY_MAX_MS; i > THROTTLE_SERVO_UNLOCK_DELAY_MIN_MS; i-=THROTTLE_SERVO_UNLOCK_DELAY_STEP_MS)
	{
		for (j = 0; j < THROTTLE_SERVO_UNLOCK_TRY_CNT; j++)
		{
			servo_set_pos_us(&throttle->servo, throttle->servo.position_min_us);
			delay_ms(i);
			servo_set_pos_us(&throttle->servo, throttle->servo.position_max_us);
			delay_ms(i);
		}
	}
		
	servo_disable(&throttle->servo);
}

//================================================================
static ErrorStatus throttle_current_sensor_check(throttle_t * throttle)
{
	int32_t zero_current_mv;
	
	zero_current_mv = ADC_MV(*throttle->servo_current_sensor_adc_val_adr);
		
	if (zero_current_mv > THROTTLE_SERVO_CURRENT_SENSOR_MAX_NORMAL_MV || 
			zero_current_mv < THROTTLE_SERVO_CURRENT_SENSOR_MIN_NORMAL_MV)
	{
		return ERROR;
	}
	return SUCCESS;
}

//================================================================
static void throttle_current_sensor_set_zero(throttle_t * throttle)
{
	throttle->servo_current_sensor_adc_val_zero = *throttle->servo_current_sensor_adc_val_adr;
}

//================================================================
static uint32_t throttle_servo_current_get(throttle_t * throttle)
{	
	int32_t adc_without_zero;
	
	adc_without_zero = *throttle->servo_current_sensor_adc_val_adr - throttle->servo_current_sensor_adc_val_zero;
	
	if (adc_without_zero < 0) adc_without_zero = 0;
	
	return ACS713_MA(ADC_MV(adc_without_zero));
}

//================================================================
//Поиск минимального и максимального положения в мкс подключённой сервомашинки
//РАБОТАЕТ и ОТТЕСТИРОВАНА
void throttle_find_min_max_servo_position(throttle_t * throttle)
{	
	#define THROTTLE_SERVO_MAX_PWM_LENTH_US 5000
	#define THROTTLE_SERVO_NORMAL_CURRENT_MIN_MA 100
	#define THROTTLE_SERVO_POSITION_MIN_MAX_SAFE_DELTA_US 50 //запас конец
	
	servo_reset_settings(&throttle->servo);
	servo_enable(&throttle->servo);
	
	uint32_t i;
	uint32_t max_pwm_lenth_us, position_min_us = 0, position_max_us = 0;
	
	//максимальная длина положительного импульса управления
	if (throttle->servo.period_us < THROTTLE_SERVO_MAX_PWM_LENTH_US)
	{
		max_pwm_lenth_us = throttle->servo.period_us;
	}
	else
	{
		max_pwm_lenth_us = THROTTLE_SERVO_MAX_PWM_LENTH_US;
	}
	
	//поставим машинку в максимальное положение
	for (i = 0; i < max_pwm_lenth_us; i++)
	{
		servo_set_pos_us_nocheck(&throttle->servo, i);
		delay_ms(1);
	}
	
	delay_ms(1000);
	
	//найдём минимальное положение, ориентируясь по току
	for (i = 0; i < max_pwm_lenth_us; i++)
	{
		servo_set_pos_us_nocheck(&throttle->servo, i);
		delay_ms(5);
		
		if (throttle_servo_current_get(throttle) > THROTTLE_SERVO_NORMAL_CURRENT_MIN_MA)
		{
			position_min_us = i + THROTTLE_SERVO_POSITION_MIN_MAX_SAFE_DELTA_US;
			break;
		}
	}
	
	//поставим машинку в минимальное положение
	servo_set_pos_us_nocheck(&throttle->servo, throttle->servo.position_min_us);
	delay_ms(1000);
	servo_set_pos_us_nocheck(&throttle->servo, 0); //и "выключим"
	delay_ms(200);
	
	//найдём максимальное положение, ориентируясь по току
	for (i = max_pwm_lenth_us; i > 1 ; i--)
	{
		servo_set_pos_us_nocheck(&throttle->servo, i);
		delay_ms(5);
		
		if (throttle_servo_current_get(throttle) > THROTTLE_SERVO_NORMAL_CURRENT_MIN_MA)
		{
			position_max_us = i - THROTTLE_SERVO_POSITION_MIN_MAX_SAFE_DELTA_US;
			break;
		}
	}
	
	if (!position_min_us || !position_max_us || position_min_us > max_pwm_lenth_us || position_max_us > max_pwm_lenth_us)
	{
		servo_reset_settings(&throttle->servo);
		//throttle->errors.autosettings_servo = SET;
		return;
	}
	
	//применим изменения
	throttle->servo.position_min_us = position_min_us;
	throttle->servo.position_max_us = position_max_us;
	
	//поставим машинку посередине	
	servo_set_pos(&throttle->servo, 0);
}

//================================================================
//функция автокалибровки минимума и максимума положения машинки газа для полностью открытой и закрытой дроссельной заслонки
//будет работать только если машинка на полном ходе заслонки работает линейно
//НЕДОПИСАНА и НЕ ОТТЕСТИРОВАНА
void throttle_auto_calibration(throttle_t * throttle)
{	
	servo_enable(&throttle->servo);
	
	servo_set_pos_us(&throttle->servo, throttle->servo.position_min_us);
	delay_ms(1000);
	
	uint32_t i, servo_current, servo_position_min_us = 0, servo_position_max_us = 0;	
	
	for (i = throttle->servo.position_min_us; i < throttle->servo.position_max_us; i++)
	{
		servo_set_pos_us(&throttle->servo, i);
		
		servo_current = throttle_servo_current_get(throttle);
		
		/*DBG*/ //test_throttle_send_to_pc(i, servo_current);
		
		if (!servo_position_min_us && servo_current < 10)
		{
			if (throttle->servo.position_min_us == i)
			{
				//throttle->errors.autosettings_servo = SET;
				servo_disable(&throttle->servo);
				return;
			}
			servo_position_min_us = i; //сделать поиск в плюс от этой точки чтобы было постоянно < тока
		}
		if (servo_position_min_us && !servo_position_max_us && servo_current > 50)
		{
			servo_position_max_us = i - 1; //сделать поиск в плюс от этой точки чтобы было постоянно > тока
		}
	}

	if (!servo_position_max_us)
	{
		//throttle->errors.autosettings_servo = SET;
		servo_disable(&throttle->servo);
		return;
	}
	
	throttle->servo.position_min_us = servo_position_min_us;
	throttle->servo.position_max_us = servo_position_max_us;
	
	servo_disable(&throttle->servo);
}
