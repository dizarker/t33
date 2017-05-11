//Pressure meter MS4525DO
//need: i2c.h
//Журавлев ЕА

#ifndef _MS4525DO_H_
#define _MS4525DO_H_

#define MS5425DO_TIMEOUT_CLEAR_FL_ERROR_MS 200

//================================================================
typedef struct
{
	uint8_t no_connect;	 	//датчик не подключен
	uint8_t bus_erorr;		//ошибка на линии (например, плохой контакт).
	uint8_t old_data;			//пришли старые данные (новые не успели измерится)
	uint8_t device_fault;	//датчик сообщил об ошибки
	uint8_t bus_lock;
} ms5425do_errors_t;

//================================================================
typedef struct
{
	i2c_t * i2c;
	uint32_t i2c_f;
	uint8_t i2c_address;
	uint8_t MS4525DO_OutputType;
	uint8_t MS4525DO_PressureRange;
	uint8_t MS4525DO_PressureType;
	uint8_t msg[2];
	int32_t value_mpsi;
	ms5425do_errors_t errors;
	uint8_t fl_update;
	uint8_t fl_error;
	
	timeout_t timeout_clear_fl_error;
} ms5425do_t;

//================================================================
enum
{
	MS4525DO_OutputTypeA,
	MS4525DO_OutputTypeB
};

//================================================================
enum
{
	MS4525DO_PressureRange1 = 1,
	MS4525DO_PressureRange2 = 2,
	MS4525DO_PressureRange5 = 5,
	MS4525DO_PressureRange15 = 15,
	MS4525DO_PressureRange30 = 30,
	MS4525DO_PressureRange50 = 50,
	MS4525DO_PressureRange100 = 100,
	MS4525DO_PressureRange150 = 150,
};

//================================================================
enum
{
	MS4525DO_PressureTypeAbsolute,
	MS4525DO_PressureTypeDifferencial,
	MS4525DO_PressureTypeGauge,
	MS4525DO_PressureTypeCompound,
	MS4525DO_PressureTypeVacuum
};

//================================================================
void ms4525do_init(ms5425do_t * sensor);
uint8_t ms4525do_start_getting(ms5425do_t * sensor);
void ms4525do_task(ms5425do_t * sensor);

#endif
