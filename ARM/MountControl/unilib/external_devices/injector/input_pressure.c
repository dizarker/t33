//Орлан. ЭБУ. Впускной коллектор.
//Журавлев ЕА

#include  "include.h"

//================================================================
#define ERR_INC(error) ((error == 0xFFFFFFFF) ? error : error++)

//================================================================
void input_pressure_init(input_pressure_t* input_pressure)
{		
	//PRESSURE SENSOR
	ms4525do_init(&input_pressure->sensor);
	
	//IRQ
	input_pressure_irq_init(input_pressure);	
}


//================================================================
void input_pressure_get_value_irq(input_pressure_t* input_pressure)
{
	static INPUT_PRESSURE_SENSOR_VALUE_TYPE value;
	
	ms4525do_task(&input_pressure->sensor);
	
	if (input_pressure->sensor.fl_error)
	{
		input_pressure->sensor.fl_error = RESET;
		
		ERR_INC(input_pressure->errors.sensor_not_response);
	}
	else
	{
		if (input_pressure->sensor.fl_update)
		{
			input_pressure->sensor.fl_update = RESET;
			
			value = input_pressure->sensor.value_mpsi;
			
			//Провера на загрязнение
			if (value > input_pressure->in.sensor_dirty_level_mpsi)
			{
				if (input_pressure->sensor_dirty_level_wait_cnt++ > input_pressure->in.sensor_dirty_level_wait_cnt_max)
				{
					ERR_INC(input_pressure->errors.sensor_dirty);
					input_pressure->sensor_dirty_level_wait_cnt = 0;
				}
			}
			else
			{
				input_pressure->sensor_dirty_level_wait_cnt = 0;
			}
			
			//Управляющее воздействие
			if (value < input_pressure->in.sensor_ignition_level_mpsi && !input_pressure->fl_action_alredy)
			{
				input_pressure->fl_action_alredy = SET;
				
				//Запуск зажигания
				//ignition_pulse(input_pressure->ignition, input_pressure->in.ignition_delay_us);
	
				//Запуск форсунки
				//atomizer_pulse(input_pressure->atomizer, input_pressure->in.atomizer_delay_us, input_pressure->in.atomizer_volume_pl);
			}
			else
			{
				input_pressure->fl_action_alredy = RESET;
			}
		}
		else
		{
			ERR_INC(input_pressure->errors.sensor_not_response);
		}
		
	}
	
	fifo_put_block(&input_pressure->sensor_value_mpsi_fifo, (uint8_t *)&value, INPUT_PRESSURE_SENSOR_VALUE_SIZE);
	
	ms4525do_start_getting(&input_pressure->sensor);
}
