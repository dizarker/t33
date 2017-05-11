//Орлан. ЭБУ. Впускной коллектор.
//Журавлев ЕА

#include  "include.h"

#define ERR_INC(error) ((error == 0xFFFFFFFF) ? error : error++)
#define TIM_CNT_TO_HZ(f_hz, cnt) (f_hz/cnt)
#define TIM_CNT_TO_HZ_ACCURATE(f_hz, cnt) (FROUND((float)f_hz/cnt))
	
//Скольки импульсов датчика не считать ошибкой отсутвия импульсов от другого датчика
#define SENS_1PULSE_CNT_ERR 3
#define SENS_7PULSE_CNT_ERR 9

//================================================================
void tachometer_init(tachometer_t* tachometer)
{		
	tachometer_irq_init(tachometer);
}

//================================================================
/**
	* @proctime [stm32f4, 168MHz] 3 us
	*/
INLINE void tachomater_1pulse_irq(tachometer_t* tachometer, uint32_t cc_value) //померить время
{
	static uint32_t tim_cnt_real;
	static uint8_t even_not_even;
	
	// ======= Действия =======
	
	if (even_not_even++ % 2) //каждое второе срабатывание
	{
		//Запуск зажигания
		ignition_pulse(tachometer->ignition, tachometer->in.ignition_delay_us);
	
		//Запуск форсунки
		//atomizer_pulse(tachometer->atomizer, tachometer->in.atomizer_delay_us, tachometer->in.atomizer_volume_pl);
	}
	
	// ======= Рассчёт оборотов =======
	
	tachometer->sens_1pulse.f_average = tachometer->sens_1pulse.f_instant;
	
	if (tachometer->sens_1pulse.cc_value_last < cc_value)
	{
		tim_cnt_real = cc_value - tachometer->sens_1pulse.cc_value_last;
	}
	else
	{
		tim_cnt_real = cc_value + (tachometer->tim_capture.max_cnt - tachometer->sens_1pulse.cc_value_last);
	}
	
	tachometer->sens_1pulse.cc_value_last = cc_value;
	
	tachometer->sens_1pulse.f_instant = TIM_CNT_TO_HZ_ACCURATE(tachometer->tim_capture.f_hz, tim_cnt_real);
	
	tachometer->sens_1pulse.f_average += tachometer->sens_1pulse.f_instant;
	tachometer->sens_1pulse.f_average /= 2;
	
	// ======= Ошибки =======
	
	if (tachometer->sens_1pulse.update_cnt > SENS_1PULSE_CNT_ERR)
	{
		ERR_INC(tachometer->errors.sens_7pulse_no_work);
	}
	
	tachometer->sens_7pulse.update_cnt = 0;
	tachometer->sens_1pulse.update_cnt++;
}

//================================================================
/**
	* @proctime [stm32f4, 168MHz] 1 us
	*/
//================================================================
INLINE void tachomater_7pulse_irq(tachometer_t* tachometer, uint32_t capture_value) //померить время
{
	// ======= Ошибки =======
	
	if (tachometer->sens_7pulse.update_cnt > SENS_7PULSE_CNT_ERR)
	{
		ERR_INC(tachometer->errors.sens_1pulse_no_work);
	}
	
	tachometer->sens_1pulse.update_cnt = 0;
	tachometer->sens_7pulse.update_cnt++;
}
