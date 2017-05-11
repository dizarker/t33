//i2c, no dma, 7bit address
//Журавлев ЕА

#include "include.h"

#if F1_CHECK

//================================================================
i2c_t i2c1 = 
{
	.sfr = I2C1,
	
	.ports.SCL_GPIOx 		= GPIOB,
	.ports.SCL_GPIO_Pin = GPIO_Pin_6,
	.ports.SCL_GPIO_PinSource = GPIO_PinSource6,
	.ports.SDA_GPIOx 		= GPIOB,
	.ports.SDA_GPIO_Pin = GPIO_Pin_7,
	.ports.SDA_GPIO_PinSource = GPIO_PinSource7
};

//================================================================
i2c_t i2c2 = 
{
	.sfr = I2C2,
	
	.ports.SCL_GPIOx 		= GPIOB,
	.ports.SCL_GPIO_Pin = GPIO_Pin_10,
	.ports.SCL_GPIO_PinSource = GPIO_PinSource10,
	.ports.SDA_GPIOx 		= GPIOB,
	.ports.SDA_GPIO_Pin = GPIO_Pin_11,
	.ports.SDA_GPIO_PinSource = GPIO_PinSource11
};

#else

//================================================================
i2c_t i2c1 = 
{
	.sfr = I2C1,
	
	.ports.SCL_GPIOx 		= GPIOB,
	.ports.SCL_GPIO_Pin = GPIO_Pin_8,
	.ports.SCL_GPIO_PinSource = GPIO_PinSource8,
	.ports.SDA_GPIOx 		= GPIOB,
	.ports.SDA_GPIO_Pin = GPIO_Pin_9,
	.ports.SDA_GPIO_PinSource = GPIO_PinSource9
};

//================================================================
i2c_t i2c2 = 
{
	.sfr = I2C2,
	
	.ports.SCL_GPIOx 		= GPIOB,
	.ports.SCL_GPIO_Pin = GPIO_Pin_10,
	.ports.SCL_GPIO_PinSource = GPIO_PinSource10,
	.ports.SDA_GPIOx 		= GPIOB,
	.ports.SDA_GPIO_Pin = GPIO_Pin_11,
	.ports.SDA_GPIO_PinSource = GPIO_PinSource11
};


//================================================================
i2c_t i2c3 = 
{
	.sfr = I2C3,
	
	.ports.SCL_GPIOx 		= GPIOA,
	.ports.SCL_GPIO_Pin = GPIO_Pin_8,
	.ports.SCL_GPIO_PinSource = GPIO_PinSource8,
	.ports.SDA_GPIOx 		= GPIOC,
	.ports.SDA_GPIO_Pin = GPIO_Pin_9,
	.ports.SDA_GPIO_PinSource = GPIO_PinSource9,
};

#endif

//================================================================
void i2c_init(i2c_t* i2c);

static uint8_t i2c_prestart_procedure(i2c_t* i2c);
static void i2c_start_procedure(i2c_t* i2c);
void i2c_task(i2c_t* i2c);

uint8_t i2c_request(i2c_t* i2c);

static void i2c_isr							(i2c_t* i2c);
static void i2c_isr_error					(i2c_t* i2c);
static void i2c_bus_lock_timeout_calc		(i2c_t* i2c);
static void i2c_init_port_normal			(i2c_t* i2c);
static void i2c_init_port_unlock			(i2c_t* i2c);

static ErrorStatus i2c_bus_unlock_proc		(i2c_t* i2c);
static ErrorStatus i2c_bus_check_and_unlock	(i2c_t* i2c);
static ErrorStatus i2c_bus_error_check		(i2c_t* i2c);

//================================================================
void I2C1_EV_IRQHandler(void)	{i2c_isr(&i2c1);}	
void I2C1_ER_IRQHandler(void)	{i2c_isr_error(&i2c1);}

void I2C2_EV_IRQHandler(void)	{i2c_isr(&i2c2);}	
void I2C2_ER_IRQHandler(void)	{i2c_isr_error(&i2c2);}

#if !F1_CHECK

void I2C3_EV_IRQHandler(void)	{i2c_isr(&i2c3);}	
void I2C3_ER_IRQHandler(void)	{i2c_isr_error(&i2c3);}

#endif

//================================================================
/**
	* @description контроль и разблокировка заблокированной шиный i2c. вызывать не обязательно. i2c_bus_check_and_unlock вызывается в uint8_t i2c_request(i2c_t* i2c)
	* @warning может отнять 15us процессорного времени	
	*/
void i2c_task(i2c_t* i2c)
{
	ENTER_CRITICAL_SECTION();
	if (i2c_bus_check_and_unlock(i2c) == SUCCESS)
	{
		i2c->errors.bus_lock = RESET;
	}
	LEAVE_CRITICAL_SECTION();
}

//================================================================
/**
* @description попробовать разблокировать шину
	* @warning может отнять 40ms процессорного времени	
	*/
void i2c_bus_unlock(i2c_t* i2c)
{
	uint32_t i;
	
	for (i = 0; i < 20; i++)
	{
		if (i2c->errors.bus_lock == RESET)
		{
			return;
		}
		i2c_task(i2c);
		delay_ms(2);
	}
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max : 4.2 us
	*/
void i2c_init(i2c_t* i2c)
{	
	i2c_init_port_normal(i2c);
	
	//delay_ms(500);
	
	I2C_DeInit(i2c->sfr);
	
	switch((uint32_t)i2c->sfr)
	{
		case ((uint32_t)I2C1): 
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
			NVIC_EnableIRQ(I2C1_EV_IRQn);
			NVIC_EnableIRQ(I2C1_ER_IRQn);
			break;
		}
		case ((uint32_t)I2C2):
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
			NVIC_EnableIRQ(I2C2_EV_IRQn);
			NVIC_EnableIRQ(I2C2_ER_IRQn);
			break;
		}
		#if !F1_CHECK
		case ((uint32_t)I2C3):
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
			NVIC_EnableIRQ(I2C3_EV_IRQn);
			NVIC_EnableIRQ(I2C3_ER_IRQn);
			break;
		}
		#endif
	}
	
	if (i2c->clock_speed == 0) i2c->clock_speed = 400000;
	
	I2C_InitTypeDef I2C_InitStructure;
	
	I2C_InitStructure.I2C_ClockSpeed 						= i2c->clock_speed;
	I2C_InitStructure.I2C_Mode 									= I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle 						= I2C_DutyCycle_2;//I2C_DutyCycle_16_9;
	I2C_InitStructure.I2C_OwnAddress1 					= 0x00;
	I2C_InitStructure.I2C_Ack 									= I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress 	= I2C_AcknowledgedAddress_7bit;
	
	I2C_Init(i2c->sfr, &I2C_InitStructure);
	
	I2C_StretchClockCmd(i2c->sfr, ENABLE);
	
	I2C_ITConfig(i2c->sfr, I2C_IT_BUF, ENABLE);
	I2C_ITConfig(i2c->sfr, I2C_IT_EVT, ENABLE);
	I2C_ITConfig(i2c->sfr, I2C_IT_ERR, ENABLE);
		
	I2C_Cmd(i2c->sfr, ENABLE);
	
	i2c->fl_error = RESET;
	
	i2c->errors.bus_error 	= RESET;
	i2c->errors.no_connect 	= RESET;
	i2c->errors.bus_lock 		= RESET;
	
	i2c->processing 				= RESET;
	
	i2c->irq_case_default_cnt = 0;
	
	i2c->address_7bit				= i2c->address << 1;
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max : ? us
	*/
static uint8_t i2c_prestart_procedure(i2c_t* i2c)
{	
	if (i2c_bus_check_and_unlock(i2c) == ERROR)
	{
		i2c->fl_error = SET;
		i2c->errors.bus_lock = SET;
		return RESET;
	}
	
	if (i2c->processing)
	{
		return RESET;
	}
	
	i2c->fl_complete 		= RESET;
	i2c->fl_error 			= RESET;
	i2c->errors.bus_error	= RESET;
	i2c->errors.no_connect 	= RESET;
	i2c->errors.bus_lock	= RESET;
	i2c->processing 		= SET;
	i2c->rx.cnt 			= 0;
	i2c->tx.cnt 			= 0;
	
	return SET;
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max : ? us
	*/
static void i2c_start_procedure(i2c_t* i2c)
{
	i2c_bus_lock_timeout_calc(i2c);
	software_timer_start(&i2c->timeout_bus_lock, i2c->timeout_bus_lock.ms);
	
	I2C_GenerateSTOP(i2c->sfr, DISABLE);	
	I2C_AcknowledgeConfig(i2c->sfr, ENABLE);
	I2C_GenerateSTART(i2c->sfr, ENABLE);
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max : 3 us при нормальной работе. 15 us - если зависнет шина
	*/
uint8_t i2c_request(i2c_t* i2c)
{			
	switch (i2c->i2c_dir)
	{
		case i2c_dir_RX:
			if (!i2c->rx.size) 
			{
				return RESET;
			}
			break;
		case i2c_dir_TX:
			if (!i2c->tx.size)
			{
				return RESET;
			}
			break;
		case i2c_dir_TX_RX:
			if (!i2c->tx.size || !i2c->rx.size) 
			{
				return RESET;
			}
			break;
		default:
			return RESET;
	}
	
	if (!i2c_prestart_procedure(i2c))
	{
		return RESET;
	}
	i2c_start_procedure(i2c);
	
	return SET;
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max : 
	*/
ErrorStatus i2c_tx(i2c_t* i2c, const void * src, size_t len)
{
	i2c->tx.buffer = (uint8_t *)src;
	i2c->tx.size = len;
	i2c->i2c_dir = i2c_dir_TX;
	
	if (i2c_request(i2c))
	{
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max : 
	*/
ErrorStatus i2c_rx(i2c_t* i2c, const void * src, uint8_t len)
{
	i2c->rx.buffer = (uint8_t *)src;
	i2c->rx.size = len;
	i2c->i2c_dir = i2c_dir_RX;
	
	if (i2c_request(i2c))
	{
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max : 
	*/
ErrorStatus i2c_tx_rx(i2c_t* i2c, const void * tx_src, size_t tx_len, const void * rx_src, uint8_t rx_len)
{
	i2c->tx.buffer = (uint8_t *)tx_src;
	i2c->tx.size = tx_len;
	
	i2c->rx.buffer = (uint8_t *)rx_src;
	i2c->rx.size = rx_len;
	
	i2c->i2c_dir = i2c_dir_TX_RX;
		
	if (i2c_request(i2c))
	{
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

//================================================================
static INLINE void i2c_event_master_mode_selected(i2c_t* i2c)
{
	switch (i2c->i2c_dir)
	{
		case i2c_dir_RX: 		
			I2C_Send7bitAddress(i2c->sfr, i2c->address_7bit, I2C_Direction_Receiver); 		
			break;
		
		case i2c_dir_TX: 		
			I2C_Send7bitAddress(i2c->sfr, i2c->address_7bit, I2C_Direction_Transmitter); 
			break;
		
		case i2c_dir_TX_RX: 
			if (!i2c->tx.cnt) //ещё не сделали ТХ
			{
				I2C_Send7bitAddress(i2c->sfr, i2c->address_7bit, I2C_Direction_Transmitter); 
			}
			else
			{
				I2C_Send7bitAddress(i2c->sfr, i2c->address_7bit, I2C_Direction_Receiver); 		
			}
			break;
		
		default:
			I2C_GenerateSTOP(i2c->sfr, ENABLE);	
			i2c->processing 	= RESET;
			i2c->fl_complete	= SET;
			break;
	}
}

//================================================================
static INLINE void i2c_event_master_tranmitter_mode_selected(i2c_t* i2c)
{
	I2C_AcknowledgeConfig(i2c->sfr, ENABLE);
	ENTER_CRITICAL_SECTION();
	I2C_SendData(i2c->sfr, i2c->tx.buffer[i2c->tx.cnt++]);
	LEAVE_CRITICAL_SECTION();	
}

//================================================================
static INLINE void i2c_event_master_byte_trasmitted(i2c_t* i2c)
{
	ENTER_CRITICAL_SECTION();
	if (i2c->tx.cnt >= i2c->tx.size)
	{
		switch (i2c->i2c_dir)
		{
			case i2c_dir_TX:
				I2C_GenerateSTOP(i2c->sfr, ENABLE);
				i2c->processing 	= RESET;
				i2c->fl_complete	= SET;
				break;
			
			case i2c_dir_TX_RX:						
				I2C_GenerateSTOP(i2c->sfr, DISABLE);	
				I2C_AcknowledgeConfig(i2c->sfr, ENABLE);	
				I2C_GenerateSTART(i2c->sfr, ENABLE);
				break;
			
			default:
				I2C_GenerateSTOP(i2c->sfr, ENABLE);
				i2c->processing 	= RESET;
				i2c->fl_complete	= SET;
				break;
		}
	}
	else
	{
		I2C_SendData(i2c->sfr, i2c->tx.buffer[i2c->tx.cnt++]);
	}
	LEAVE_CRITICAL_SECTION();	
}

//================================================================
static INLINE void i2c_event_master_receiver_mode_selected(i2c_t* i2c)
{
	if (i2c->rx.size == 1)
	{
		I2C_AcknowledgeConfig(i2c->sfr, DISABLE);
		I2C_GenerateSTOP(i2c->sfr, ENABLE);
	}
}

//================================================================
static INLINE void i2c_event_master_byte_received(i2c_t* i2c)
{
	ENTER_CRITICAL_SECTION();	
	*(i2c->rx.buffer + i2c->rx.cnt++) = I2C_ReceiveData(i2c->sfr);
	
	if (i2c->rx.cnt == (i2c->rx.size - 1)) 
	{
		I2C_AcknowledgeConfig(i2c->sfr, DISABLE);
		I2C_GenerateSTOP(i2c->sfr, ENABLE);
	}
	else if (i2c->rx.cnt >= i2c->rx.size) 
	{
		//I2C_GenerateSTOP(i2c->sfr, ENABLE);
		i2c->processing 	= RESET;
		i2c->fl_complete	= SET;
	}
	LEAVE_CRITICAL_SECTION();		
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max: получить 2 байта (4 вызова этой функции): t1(0.35 us) + t2(0.4 us) + 2 * t3(0.65 us) = 2.05 us 
	*/
//uint32_t tmp;
static INLINE void i2c_isr(i2c_t* i2c)
{		
	uint_fast32_t event = I2C_GetLastEvent(i2c->sfr);
	//tmp = event;
	
	switch(event)
	{
		/*master selected*/	
		//----------------------------------------------------------------
		case I2C_EVENT_MASTER_MODE_SELECT:
			i2c_event_master_mode_selected(i2c);
			i2c->irq_case_default_cnt = 0;
			break;
		
		/*master tx*/	
		//----------------------------------------------------------------	
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:	
			i2c_event_master_tranmitter_mode_selected(i2c);
			i2c->irq_case_default_cnt = 0;
			break;	
		
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
			I2C_ITConfig(i2c->sfr, I2C_IT_BUF, DISABLE);
			break;
		
		//----------------------------------------------------------------
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
			I2C_ITConfig(i2c->sfr, I2C_IT_BUF, ENABLE);
			i2c_event_master_byte_trasmitted(i2c);
			i2c->irq_case_default_cnt = 0;
			break;
			
		/*master rx*/
		//----------------------------------------------------------------
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
			i2c_event_master_receiver_mode_selected(i2c);
			i2c->irq_case_default_cnt = 0;
			break;
		
		//----------------------------------------------------------------
		case I2C_EVENT_MASTER_BYTE_RECEIVED:
			i2c_event_master_byte_received(i2c);
			i2c->irq_case_default_cnt = 0;
			break;
			
		default:
			i2c->irq_case_default_cnt++;
			if (i2c->irq_case_default_cnt > I2C_IRQ_CASE_DEFAULT_CNT_MAX)
			{
				i2c->irq_case_default_cnt = 0;
				
				I2C_GenerateSTOP(i2c->sfr, ENABLE);
				
				I2C_SoftwareResetCmd(i2c->sfr, ENABLE);
				
				i2c->processing 	= RESET;
				i2c->fl_complete	= SET;
				
				i2c->errors.bus_error = SET;
				i2c->fl_error	= SET;
			}
			break;
	}
}

//================================================================
/**
	* @proctime [stm32f4 168MHz] t max : 1.4 us
	*/
static void i2c_isr_error(i2c_t* i2c)
{	
	if (I2C_GetFlagStatus(i2c->sfr, I2C_FLAG_BERR) || 
			I2C_GetFlagStatus(i2c->sfr, I2C_FLAG_ARLO) || 
			I2C_GetFlagStatus(i2c->sfr, I2C_FLAG_OVR)) 
	{
		I2C_ClearITPendingBit(i2c->sfr, I2C_FLAG_BERR);
		I2C_ClearITPendingBit(i2c->sfr, I2C_FLAG_ARLO);
		I2C_ClearITPendingBit(i2c->sfr, I2C_FLAG_OVR); 		
		
		i2c->errors.bus_error = SET;
	}
	if (I2C_GetFlagStatus(i2c->sfr, I2C_FLAG_AF)) 
	{
		I2C_ClearITPendingBit(i2c->sfr, I2C_FLAG_AF); 
		
		i2c->errors.no_connect = SET;
	}
	
	i2c->fl_error	= SET;
	I2C_GenerateSTOP(i2c->sfr, ENABLE);
	i2c->processing = RESET;
}

//================================================================
static void i2c_bus_lock_timeout_calc(i2c_t* i2c)
{
	float timeout_calc_ms;
	float one_tick_ms;
	
	one_tick_ms = 1000 / i2c->clock_speed;
	
	timeout_calc_ms = 9 * i2c->tx.size * one_tick_ms + 9 * i2c->rx.size * one_tick_ms + 2 * one_tick_ms + BUS_LOCK_TIME_OUT_TOLERANCE_MS;
	
	i2c->timeout_bus_lock.ms = timeout_calc_ms;
}

//================================================================
static ErrorStatus i2c_bus_error_check(i2c_t* i2c)
{

	if (i2c->errors.bus_error) 
	{
		return ERROR;
	}
	if (i2c->processing && !I2C_GetFlagStatus(i2c->sfr, I2C_FLAG_BUSY))
	{
		return ERROR;
	}
	if (!i2c->processing && I2C_GetFlagStatus(i2c->sfr, I2C_FLAG_BUSY))
	{
		return ERROR;
	}
	if ((i2c->processing || I2C_GetFlagStatus(i2c->sfr, I2C_FLAG_BUSY)))
	{
		if (software_timer(&i2c->timeout_bus_lock))
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

//================================================================
static void i2c_init_port_normal(i2c_t* i2c)
{
	//RCC SCL SDA
	rcc_gpio(i2c->ports.SCL_GPIOx, ENABLE);
	rcc_gpio(i2c->ports.SDA_GPIOx, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
#if F1_CHECK
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = i2c->ports.SCL_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(i2c->ports.SCL_GPIOx, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = i2c->ports.SDA_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(i2c->ports.SCL_GPIOx, &GPIO_InitStructure);
	
#else
	
	//SCL
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin 		= i2c->ports.SCL_GPIO_Pin;
	GPIO_InitStructure.GPIO_PuPd 		= GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(i2c->ports.SCL_GPIOx, &GPIO_InitStructure);
	
	//SDA
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin 		= i2c->ports.SDA_GPIO_Pin;
	GPIO_InitStructure.GPIO_PuPd 		= GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed  	= GPIO_Speed_50MHz;
	GPIO_Init(i2c->ports.SDA_GPIOx, &GPIO_InitStructure);

	//AF SCL SDA
	switch((uint32_t)i2c->sfr)
	{
		case ((uint32_t)I2C1): 
		{
			GPIO_PinAFConfig(i2c->ports.SCL_GPIOx, i2c->ports.SCL_GPIO_PinSource, GPIO_AF_I2C1);
			GPIO_PinAFConfig(i2c->ports.SDA_GPIOx, i2c->ports.SDA_GPIO_PinSource, GPIO_AF_I2C1);
			break;
		}
		case ((uint32_t)I2C2):
		{
			GPIO_PinAFConfig(i2c->ports.SCL_GPIOx, i2c->ports.SCL_GPIO_PinSource, GPIO_AF_I2C2);
			GPIO_PinAFConfig(i2c->ports.SDA_GPIOx, i2c->ports.SDA_GPIO_PinSource, GPIO_AF_I2C2);
			break;
		}
		case ((uint32_t)I2C3):
		{
			GPIO_PinAFConfig(i2c->ports.SCL_GPIOx, i2c->ports.SCL_GPIO_PinSource, GPIO_AF_I2C3);
			GPIO_PinAFConfig(i2c->ports.SDA_GPIOx, i2c->ports.SDA_GPIO_PinSource, GPIO_AF_I2C3);
			break;
		}
	}
	
	#endif
}

//================================================================
static void i2c_init_port_unlock(i2c_t* i2c)
{
	//RCC SCL SDA
	rcc_gpio(i2c->ports.SCL_GPIOx, ENABLE);
	rcc_gpio(i2c->ports.SDA_GPIOx, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

#if F1_CHECK
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = i2c->ports.SCL_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(i2c->ports.SCL_GPIOx, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = i2c->ports.SDA_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(i2c->ports.SCL_GPIOx, &GPIO_InitStructure);
	
#else
	
	//SCL
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 		= i2c->ports.SCL_GPIO_Pin;
	GPIO_InitStructure.GPIO_PuPd 		= GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(i2c->ports.SCL_GPIOx, &GPIO_InitStructure);
	
	//SDA
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin 		= i2c->ports.SDA_GPIO_Pin;
	GPIO_InitStructure.GPIO_PuPd 		= GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed  	= GPIO_Speed_50MHz;
	GPIO_Init(i2c->ports.SDA_GPIOx, &GPIO_InitStructure);
	
#endif
}


//================================================================
static ErrorStatus i2c_bus_unlock_proc(i2c_t* i2c)
{
	switch (i2c->bus_unlock.step)
	{
		case 0: //start unlock			
			i2c_init_port_unlock(i2c);
		
			GPIO_SetBits(i2c->ports.SCL_GPIOx, i2c->ports.SCL_GPIO_Pin);
			GPIO_SetBits(i2c->ports.SDA_GPIOx, i2c->ports.SDA_GPIO_Pin);
			
			software_timer_start(&i2c->bus_unlock.timeout_unlock_step, BUS_UNLOCK_CLK_STEP_MS);
			i2c->bus_unlock.step++;
			break;
		
		case 1: //clk
			if (software_timer(&i2c->bus_unlock.timeout_unlock_step))
			{
				if (GPIO_ReadInputDataBit(i2c->ports.SDA_GPIOx, i2c->ports.SDA_GPIO_Pin)) //unlock success
				{
					i2c_init_port_normal(i2c); //5us			
					I2C_SoftwareResetCmd(i2c->sfr, ENABLE);
					i2c_init(i2c);
										
					i2c->processing = RESET;
					
					i2c->bus_unlock.clk_step = 0;
					i2c->bus_unlock.step = 0;
					
					return SUCCESS;
				}
				
				if (!i2c->bus_unlock.clk_step % 2)
				{
					GPIO_ResetBits(i2c->ports.SCL_GPIOx, i2c->ports.SCL_GPIO_Pin);
				}
				else
				{
					GPIO_SetBits(i2c->ports.SCL_GPIOx, i2c->ports.SCL_GPIO_Pin);
				}
				i2c->bus_unlock.clk_step++;
				
				if (i2c->bus_unlock.clk_step > 18) //unlock fail
				{
					i2c->bus_unlock.clk_step = 0;
					i2c->bus_unlock.step = 0;
					break;
				}
			}
			else break;			
			
		default: break;
			
	}	
	return ERROR;
}

//================================================================
static ErrorStatus i2c_bus_check_and_unlock(i2c_t* i2c)
{
	if (!i2c->bus_unlock.proc_fl)
	{
		if (i2c_bus_error_check(i2c) == ERROR)
		{
			i2c->bus_unlock.proc_fl = SET;
		}
		else
		{
			return SUCCESS;
		}
	}
	
	if (i2c_bus_unlock_proc(i2c) == SUCCESS)
	{
		i2c->bus_unlock.proc_fl = RESET;
		return SUCCESS;
	}
	
	return ERROR;
}
