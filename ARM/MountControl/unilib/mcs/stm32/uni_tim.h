
#ifndef _UNI_TIM_H_
#define _UNI_TIM_H_

//================================================================
void uni_tim_base_init(TIM_TypeDef* TIMx, uint32_t period_cnt, uint32_t tim_frq_hz);
	
//================================================================
#if F1_CHECK
	void uni_tim_pwm_init(TIM_TypeDef* TIMx, volatile uint16_t * CCRx, uint32_t Pulse, uint16_t TIM_OCPolarity, uint8_t EnableOutput_fl);
#else
	void uni_tim_pwm_init(TIM_TypeDef* TIMx, volatile uint32_t * CCRx, uint32_t Pulse, uint16_t TIM_OCPolarity, uint8_t EnableOutput_fl);
#endif

//================================================================
#if F1_CHECK
	void uni_tim_ic_init(TIM_TypeDef* TIMx, volatile uint16_t * CCRx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection);
#else
	void uni_tim_ic_init(TIM_TypeDef* TIMx, volatile uint32_t * CCRx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection);
#endif

//================================================================
typedef struct
{
	TIM_TypeDef* TIMx;
	
	#if F1_CHECK
		volatile uint16_t * CCRx;
	#else
		volatile uint32_t * CCRx;
	#endif
	
	uint32_t frq_hz;
	uint32_t period_cnt;
	uint16_t TIM_OCPolarity;
} uni_tim_pwm_t;

#endif
