// Servo-motors pwm control

/*
*	WARNING! IF USE HSE, NEED TRUE DEFINE "HSE_VALUE"
*	EXAMPLE: HSE_VALUE=16000000
*/

#ifndef _SERVO_H_
#define _SERVO_H_


//================================================================
#define LINEAR_DEPEND_Y(x, x1, x2, y1, y2) ((((float)x-x1)/(x2-x1))*(y2-y1) + y1)

typedef enum 
{
	SERVO_ORIENTATION_NORMAL = 0, 
	SERVO_ORIENTATION_TOGGLE = 1
} servo_orientation_t;

#define SERVO_POSITION_MIN 							-1.0f
#define SERVO_POSITION_MAX 							1.0f

#define SERVO_PERIOD_US_DEFAULT 				20000
#define SERVO_POSITION_MIN_US_DEFAULT 	900
#define SERVO_POSITION_MAX_US_DEFAULT 	2100
#define SERVO_ORIENTATION_DEFAULT 			SERVO_ORIENTATION_NORMAL
#define SERVO_POSITION_DEFAULT 					0
#define SERVO_POSITION_US_DEFAULT 			1500
#define SERVO_ENABLE_DEFAULT 						RESET

//================================================================
typedef struct
{
	TIM_TypeDef*							tim;
	uint8_t										tim_ch;
	
	uint32_t									period_us;
	uint32_t									position_min_us;
	uint32_t									position_max_us;
	servo_orientation_t				orientation;
	
	float											position;			//-1...1
	uint32_t									position_us;	//position_min_us...position_max_us
	
	FlagStatus								enable;
}
servo_t;

//================================================================

//инициализация
ErrorStatus servo_init (servo_t* servo, TIM_TypeDef* tim, uint8_t tim_ch);

//включить управляющее воздействия на серво машинку
ErrorStatus servo_enable (servo_t* servo);

//выключить управляющее воздействие на серво машинку
ErrorStatus servo_disable (servo_t* servo);

//установить текущую позицию: от -1 до 1
ErrorStatus servo_set_pos (servo_t* servo, float position);

//установить текущую позицию: в мкс
ErrorStatus servo_set_pos_us (servo_t* servo, uint32_t position_us);

//установить текущую позицию без проверок: в мкс
void servo_set_pos_us_nocheck (servo_t* servo, uint32_t position_us);

//получить текущую позицию: от -1 до 1
float servo_get_pos (servo_t* servo);

//получить текущую позицию: в мкс
uint32_t servo_get_pos_us (servo_t* servo);

//установить минимальное положение: в мкс
ErrorStatus servo_set_min_us(servo_t* servo, uint32_t position_us);

//установить максимальное положение: в мкс
ErrorStatus servo_set_max_us(servo_t* servo, uint32_t position_us);

//установить период: в мкс
ErrorStatus servo_set_period(servo_t* servo, uint32_t period_us);

//установить ориентацию машинки
void servo_set_orientation(servo_t* servo, servo_orientation_t orientation);

//настройки по умолчанию
void servo_reset_settings(servo_t* servo);

#endif
