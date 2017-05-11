//Pressure meter MS4525DO
//need: i2c.h
//Журавлев ЕА

#include "include.h"

void ms4525do_init				(ms5425do_t* sensor);
uint8_t ms4525do_start_getting	(ms5425do_t* sensor);
uint8_t ms4525do_get_mpsi		(ms5425do_t* sensor);

//================================================================
void ms4525do_init(ms5425do_t* sensor)
{
	sensor->i2c->rx.buffer 		= sensor->msg;
	sensor->i2c->rx.size 		= sizeof(sensor->msg);
	sensor->i2c->address 		= sensor->i2c_address;
	sensor->i2c->clock_speed 	= sensor->i2c_f;
	
	sensor->i2c->i2c_dir 		= i2c_dir_RX;
	
	sensor->errors.no_connect = 0;
	
	i2c_init(sensor->i2c);
	
	sensor->fl_error = SET;
		
	software_timer_start(&sensor->timeout_clear_fl_error, MS5425DO_TIMEOUT_CLEAR_FL_ERROR_MS);
}

//================================================================
uint8_t ms4525do_start_getting(ms5425do_t * sensor)
{		
	if(!i2c_request(sensor->i2c))
	{
		return RESET;
	}
	return SET;
}

//================================================================
#define MIN_DIGITAL(type_out) (((type_out) == MS4525DO_OutputTypeA) ? (0x666)		:	(0x333))
#define MAX_DIGITAL(type_out) (((type_out) == MS4525DO_OutputTypeA) ? (0x3332)	: (0x399A))
#define CONVERT_TO_PSI(digital_out, min_p, max_p, type_out)  ((((digital_out - MIN_DIGITAL(type_out)) * (max_p - min_p)) / MAX_DIGITAL(type_out)) + min_p)

#define STATUS_MASK 		0xC0
#define STATUS_NORMAL 		0x00
#define STATUS_OLD_DATA 	0x02
#define STATUS_FAULT 		0x03

//================================================================
void ms4525do_check_errors(ms5425do_t * sensor)
{
	uint8_t status = (sensor->msg[0] & STATUS_MASK)>>6;
	
	if (status == STATUS_OLD_DATA)
	{
		sensor->fl_error = SET;
		sensor->errors.old_data = SET;
	}
	else 
	{
		sensor->errors.old_data = RESET;
	}
	
	if (status == STATUS_FAULT)
	{
		sensor->fl_error 			= SET;
		sensor->errors.device_fault = SET;
	}
	else 
	{
		sensor->errors.device_fault = RESET;
	}
}

//================================================================
uint32_t ms4525do_get_multiplied_psi(ms5425do_t * sensor, uint16_t multiplied_value)
{
	int32_t digital_out = sensor->msg[1] | (sensor->msg[0] & ~STATUS_MASK) << 8;
	
	switch(sensor->MS4525DO_PressureType)
	{
		case (MS4525DO_PressureTypeAbsolute):
		{
			return CONVERT_TO_PSI(digital_out, 0, sensor->MS4525DO_PressureRange*multiplied_value, sensor->MS4525DO_OutputType);
		}
		case MS4525DO_PressureTypeDifferencial:
		{
			return CONVERT_TO_PSI(digital_out, -sensor->MS4525DO_PressureRange*multiplied_value, sensor->MS4525DO_PressureRange*multiplied_value, sensor->MS4525DO_OutputType);
		}
		case MS4525DO_PressureTypeGauge:
		{
			return CONVERT_TO_PSI(digital_out, 0, sensor->MS4525DO_PressureRange*multiplied_value, sensor->MS4525DO_OutputType);
		}
		case MS4525DO_PressureTypeCompound:
		{
			return CONVERT_TO_PSI(digital_out, -15*multiplied_value, 0, sensor->MS4525DO_OutputType);
		}
		case MS4525DO_PressureTypeVacuum:
		{
			return CONVERT_TO_PSI(digital_out, -15*multiplied_value, sensor->MS4525DO_PressureRange*multiplied_value, sensor->MS4525DO_OutputType);
		}
		default: 
		{
			return 0;
		}
	}
}

//================================================================
void ms4525do_task(ms5425do_t * sensor)
{
	if (sensor->i2c->fl_error)
	{		
		sensor->fl_error = SET;
		sensor->errors.bus_erorr = sensor->i2c->errors.bus_error;
		sensor->errors.no_connect = sensor->i2c->errors.no_connect;
		sensor->errors.bus_lock = sensor->i2c->errors.bus_lock;
		
		software_timer_start(&sensor->timeout_clear_fl_error, MS5425DO_TIMEOUT_CLEAR_FL_ERROR_MS);
	}
	else
	{
		if (software_timer(&sensor->timeout_clear_fl_error))
		{
			sensor->fl_error = RESET;
			sensor->errors.bus_erorr = sensor->i2c->errors.bus_error;
			sensor->errors.no_connect = sensor->i2c->errors.no_connect;
			sensor->errors.bus_lock = sensor->i2c->errors.bus_lock;
		}
	}
	
	if (!sensor->i2c->fl_complete)
	{
		return;
	}
	sensor->i2c->fl_complete = RESET;
	
	//Смотрим флаги состояния датчика
	ms4525do_check_errors(sensor);
	
	//Вычисляем давление
	sensor->value_mpsi = ms4525do_get_multiplied_psi(sensor, 1000); //1000 = psi/mpsi
	
	sensor->fl_update = SET;

}
