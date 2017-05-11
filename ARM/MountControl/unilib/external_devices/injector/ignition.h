//Орлан. ЭБУ. Зажигание.
//Журавлев ЕА

#ifndef _IGNITION_H_
#define _IGNITION_H_

//NEED TRUE DEFINE "HSE_VALUE"
//NEED DEFINE "IGNITION_PIN_PWR" //пин включения/выключения зажигания

/*
#include "rcc.c"
#include "global_macro.h"
#include "stm32f4xx_rcc.c"
#include "stm32f4xx_tim.h"
*/

/*
FG36 INIT

#define IGNITION_PIN_PULSE A,15,L,ALT_OUTPUT_PUSH_PULL,SPEED_50MHZ
#define IGNITION_PIN_PWR C,5,L,OUTPUT_PUSH_PULL,SPEED_50MHZ

ignition_t ignition_fg36 =
{
	.in.pulse_delay_after_signal_us 	= 0,
	.in.pulse_signal_length_us 				= 50,
	.settings.TIMx 										= TIM2,
	.settings.CCRx 										= &(TIM2->CCR1),
	.settings.TIM_OCPolarity 					= TIM_OCPolarity_High,
};

pin_init(IGNITION_PIN_PULSE);
pin_init_af(IGNITION_PIN_PULSE, GPIO_AF_TIM2);

ignition_init(&ignition_fg36);
*/

//================================================================
typedef struct
{
	struct 
	{
		volatile uint32_t pulse_signal_length_us;				//длительность сигнала для блока зажигания
		volatile uint32_t pulse_delay_after_signal_us;	//задержка искры после сигнала
	} in;
	
	struct 
	{
		volatile uint32_t pulse_cnt;								//кол-во искр с начала работы
	} out;

	struct
	{	
		uint8_t set_too_small_delay;		//невозможно так быстро подать искру, искра подастся максимально быстро
	} errors;
	
	struct 
	{
		TIM_TypeDef* TIMx;								//тамер - выход сигнала
		#if F1_CHECK
			volatile uint16_t * CCRx;				//канал таймера - выход сигнала CCR1, CCR2, CCR3, CCR4
		#else
			volatile uint32_t * CCRx;				//канал таймера - выход сигнала CCR1, CCR2, CCR3, CCR4
		#endif
		uint16_t TIM_OCPolarity;
	} settings;

} ignition_t;

//================================================================
void ignition_init(ignition_t* ignition);
void ignition_pwr_on(void);
void ignition_pwr_off(void);
void ignition_pulse(ignition_t* ignition, uint32_t delay_us);

//================================================================
/*
*	Блок зажигания: "SAITO for troke gasoline engine".
*	Управляется отрицательным импульсом длительностью >30 us можно больше.
*	Искра даётся блоком через x us (зависит от частоты) после положительного перепада импульса.
*
*	---			 			--- сигнал
*		|						|
*		|___________|	*
*				>30 us		^
*									|
*								искра
*
*	После выхода микроконтроллера сигнал интверируется оптопарой.
*
*	Блок зажигания корректно работает, если частота следования импульсов больше 7 Гц.
*
* Реализация: таймер в режиме one_pulse выдаёт сигнал на ногу контроллера.
*/

#endif
