
#ifndef _EV_MATH_H_
#define _EV_MATH_H_

#define EV_MATH_F_NORM_DEFAULT 				18	//	f = 8.0
#define EV_MATH_F_NORM_MIN_DEFAULT 		14	//	f = 5.0
#define EV_MATH_ISO_NORM_MAX_DEFAULT		3		//	iso = 800
#define EV_MATH_EV_CORRECTION_DEFAULT	0		//	ev_correction = 0
#define EV_MATH_S_MIN_ALLOW_DEFAULT 		30	//	s = 1/1000

//================================================================
typedef struct //выбирается в экспочислах с шагом 1/3 (стандартный экспоряд умноженый на 3)
{
	int8_t f_norm;					//диафрагма максимальной резкости
	int8_t f_norm_min;			//допустимое открытие диафрагмы
	int8_t iso_norm_max;		//допустимоге увеличение исо
	int8_t ev_correction;   //коррекция экспозиции
	//int8_t s_min_allow;		//максимальная выдержка
} ev_math_ev_settings_t;

typedef struct
{
	int8_t module_no_init;							//модуль не инициализирован
	int8_t set_ev_settings_value_error;	//ошибка значений настройки экспозиции
	int8_t too_ligth;										//переизбыток света
	int8_t too_dark;										//недостаток света
} ev_math_errors_t;

typedef struct
{
	int8_t f;
	int8_t s;
	int8_t iso;
} ev_math_cam_settings_t;

#pragma pack(push,1)

typedef struct
{
	int8_t min;
	int8_t max;
	int8_t step;
	int8_t denom;
	float ev_step;
} ev_math_cam_settings_group_t;

typedef struct
{
	ev_math_cam_settings_group_t f;
	ev_math_cam_settings_group_t s;
	ev_math_cam_settings_group_t iso;
} ev_math_cam_settings_limits_t;

#pragma pack(pop)

int8_t ev_math_init(ev_math_cam_settings_limits_t *cam_settings_limits_set);
int8_t ev_math_calc_cam_settings(float ev, ev_math_cam_settings_t *cam_settings, ev_math_errors_t *errors);
uint8_t ev_math_set_ev_settings(ev_math_ev_settings_t *ev_settings_set, ev_math_errors_t *errors);

#endif
