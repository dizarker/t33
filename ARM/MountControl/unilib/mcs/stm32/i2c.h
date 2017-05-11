//i2c, no dma, 7bit address
/*
v2 	i2c_request

v3 	разбитие на функции rx, tx, rx_tx

v4 
Сделана защита от линии от блокировки. Цикл работы: детектирование блокировки линии -> вывод линии из заблокированного состояни -> переинициализация i2c

 - Инициализация портов перенесена в модуль. GPIO_Pin, GPIO_PinSource и GPIOx для SDA и SCL указываются в i2c_t.ports. ...
 - Добавлена внутренняя функция модуля: i2c_bus_check_and_unlock, которая разблокирует линию, если она заблокирована за N < 20 вызовов.
 - Добавлена внешняя функция i2c_task(i2c_t* i2c), вызывает i2c_bus_check_and_unlock.
*/

//need systick_timer.c/h, rcc.c/h, atomic.h, stm32f4xx.c/h, stm32f4xx_i2c.c/h, stm32f4xx_rcc.c/h, stm32f4xx_gpio.c/h, delay.c/h

//Журавлев ЕА


#ifndef _I2C_H_
#define _I2C_H_
//================================================================
#define BUS_UNLOCK_CLK_STEP_MS 1
#define BUS_LOCK_TIME_OUT_TOLERANCE_MS 2 //сколько миллисекунд ещё ждать после рассчётно таймаута //для медленных устройств - потребуется увеличение этого времени
#define I2C_IRQ_CASE_DEFAULT_CNT_MAX 5

//================================================================
typedef struct
{
	volatile uint8_t		no_connect;	//Девайс не подключён
	volatile uint8_t		bus_error;	//Ошибка на линии (например, плохой контакт)
	volatile uint8_t		bus_lock;		//Линия заблокирована
} i2c_error_t;

//================================================================
typedef struct
{
	volatile uint8_t*		buffer;
	volatile uint32_t		size;
	volatile uint32_t		cnt;
} i2c_buffer_t;

//================================================================
typedef struct
{
	I2C_TypeDef* const	sfr;
	uint32_t						clock_speed;
	uint8_t							address;
	volatile uint8_t		address_7bit;
	
	i2c_buffer_t 				tx;
	i2c_buffer_t 				rx;
	
	i2c_error_t 				errors;
	
	volatile	uint8_t		processing;
	
	volatile	uint8_t		fl_error;
	volatile	uint8_t		fl_complete;
	
	volatile	uint8_t		i2c_dir;
	
	struct
	{
		GPIO_TypeDef* SCL_GPIOx;
		uint16_t 			SCL_GPIO_Pin;
		uint8_t 			SCL_GPIO_PinSource;
		GPIO_TypeDef* SDA_GPIOx;
		uint16_t 			SDA_GPIO_Pin;
		uint8_t 			SDA_GPIO_PinSource;
	} ports;
	
	
	timeout_t timeout_bus_lock;
	
	struct
	{
		FlagStatus proc_fl;
		uint16_t step;
		uint16_t clk_step;
		timeout_t timeout_unlock_step;
	}bus_unlock;
	
	uint32_t irq_case_default_cnt;
	
} i2c_t;

//================================================================
extern i2c_t i2c1;
extern i2c_t i2c2;

#if !F1_CHECK

extern i2c_t i2c3;

#endif

//================================================================
enum
{
	i2c_dir_RX 		= 1,
	i2c_dir_TX 		= 2,
	i2c_dir_TX_RX = 3
};

//================================================================
void i2c_init(i2c_t* i2c);

uint8_t i2c_request(i2c_t* i2c); //поддержка v1

void i2c_bus_unlock(i2c_t* i2c);
void i2c_task(i2c_t* i2c);
ErrorStatus i2c_tx(i2c_t* i2c, const void * src, size_t len);
ErrorStatus i2c_rx(i2c_t* i2c, const void * src, uint8_t len);
ErrorStatus i2c_tx_rx(i2c_t* i2c, const void * tx_src, size_t tx_len, const void * rx_src, uint8_t rx_len);

#endif
