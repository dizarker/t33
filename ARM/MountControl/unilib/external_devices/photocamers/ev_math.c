//Математика рассчёта настроек экспозиции для аэрофтосъёмки
#include "include.h"

//****************************************************************
//prototypes of functions
//****************************************************************
int8_t ev_math_init(ev_math_cam_settings_limits_t *cam_settings_limits_set);
int8_t ev_math_calc_cam_settings(float ev, ev_math_cam_settings_t *cam_settings, ev_math_errors_t *errors);
uint8_t ev_math_set_ev_settings(ev_math_ev_settings_t *ev_settings_set, ev_math_errors_t *errors);

static float ev_calc(ev_math_cam_settings_t *cam_settings);
static uint8_t compare(float val_1, float val_2, float delta);
static uint8_t find_optimal_parametr(int8_t * value, int8_t step, int8_t limit, ev_math_cam_settings_t * cam_settings, float ev, float delta);
static void reset_errors(ev_math_errors_t *errors);

//****************************************************************
//module global variables
//****************************************************************
static ev_math_ev_settings_t	ev_settings = {EV_MATH_F_NORM_DEFAULT, EV_MATH_F_NORM_MIN_DEFAULT, EV_MATH_ISO_NORM_MAX_DEFAULT, EV_MATH_EV_CORRECTION_DEFAULT};
static ev_math_cam_settings_limits_t	cam_settings_limits;
static int8_t fl_module_init = RESET;

//****************************************************************
//functions
//****************************************************************

//================================================================
int8_t ev_math_init(ev_math_cam_settings_limits_t *cam_settings_limits_set)
{
	
	//Проверяем: сможет ли алгоритм работать с полученными значениями
	if (cam_settings_limits_set->f.max < cam_settings_limits_set->f.min || cam_settings_limits_set->f.denom == 0 || cam_settings_limits_set->f.step == 0 ||
			cam_settings_limits_set->s.max < cam_settings_limits_set->s.min || cam_settings_limits_set->s.denom == 0 || cam_settings_limits_set->s.step == 0 ||
			cam_settings_limits_set->iso.max < cam_settings_limits_set->iso.min || cam_settings_limits_set->iso.denom == 0 || cam_settings_limits_set->iso.step == 0)
	{
		return RESET;
	}
	
	memcpy(&cam_settings_limits, cam_settings_limits_set, sizeof(cam_settings_limits));
	
	//Считаем минимальный шаг экспозиции для каждого параметра
	cam_settings_limits.f.ev_step = 	(float)cam_settings_limits.f.step / cam_settings_limits.f.denom;
	cam_settings_limits.s.ev_step = 	(float)cam_settings_limits.s.step / cam_settings_limits.s.denom;
	cam_settings_limits.iso.ev_step =	(float)cam_settings_limits.iso.step / cam_settings_limits.iso.denom;
	
	fl_module_init = SET;
	
	return SET;
}

//================================================================
int8_t ev_math_calc_cam_settings(float ev, ev_math_cam_settings_t *cam_settings, ev_math_errors_t *errors)
{
	/*
	Последовательность действий:
	1) f = f_norm, s = s_min, iso = iso_min
	2) если ev > ev_set → увеличиваем f до f_max → выход
	3) уменьшаем s до s_min_allow → если достаточно, выход
	4) увеличиваем iso до iso_norm_max → если достаточно, выход
	5) уменьшаем f до f_norm_min → если достаточно, выход
	6) увеличиваем iso до iso_max → если достаточно, выход
	7) уменьшаем f до f_min → выход
	*/
	
	reset_errors(errors);
	
	if (!fl_module_init) 
	{
		errors->module_no_init = SET;
		return RESET;
	}
	
	float cam_ev;
	
	ev = ev + ev_settings.ev_correction;
	
	//1
	cam_settings->f = ev_settings.f_norm;
	cam_settings->s = cam_settings_limits.s.max;
	cam_settings->iso = cam_settings_limits.iso.min;
	
	cam_ev = ev_calc(cam_settings);
	
	if (compare(ev, cam_ev, cam_settings_limits.f.ev_step))
	{
		return SET;
	}
	
	//2
	if (ev > cam_ev)
	{
		if(find_optimal_parametr(&cam_settings->f, cam_settings_limits.f.step, cam_settings_limits.f.max, cam_settings, ev, cam_settings_limits.f.ev_step))
		{		
			return SET;
		}
		errors->too_ligth = SET;
		return RESET;
	}
	//3
	if (find_optimal_parametr(&cam_settings->s, -cam_settings_limits.s.step, EV_MATH_S_MIN_ALLOW_DEFAULT, cam_settings, ev, cam_settings_limits.s.ev_step))
	{
		return SET;
	}
	//4
	if (find_optimal_parametr(&cam_settings->iso, cam_settings_limits.iso.step, ev_settings.iso_norm_max, cam_settings, ev, cam_settings_limits.iso.ev_step))
	{
		return SET;
	}
	//5
	if (find_optimal_parametr(&cam_settings->f, -cam_settings_limits.f.step, ev_settings.f_norm_min, cam_settings, ev, cam_settings_limits.f.ev_step))
	{
		return SET;
	}
	//6
	if (find_optimal_parametr(&cam_settings->iso, cam_settings_limits.iso.step, cam_settings_limits.iso.max, cam_settings, ev, cam_settings_limits.iso.ev_step))
	{
		return SET;
	}
	//5
	if (find_optimal_parametr(&cam_settings->f, -cam_settings_limits.f.step, cam_settings_limits.f.min, cam_settings, ev, cam_settings_limits.f.ev_step))
	{
		return SET;
	}
	
	errors->too_dark = SET;
	
	return RESET;
}

//================================================================
uint8_t ev_math_set_ev_settings(ev_math_ev_settings_t *ev_settings_set, ev_math_errors_t *errors)
{	
	
	reset_errors(errors);
	
	if (!fl_module_init)
	{
		errors->module_no_init = SET;
		return RESET;
	}
	
	if (ev_settings_set->f_norm > cam_settings_limits.f.max || ev_settings_set->f_norm < cam_settings_limits.f.min ||
			ev_settings_set->f_norm_min > cam_settings_limits.f.max || ev_settings_set->f_norm_min < cam_settings_limits.f.min ||
			ev_settings_set->iso_norm_max > cam_settings_limits.iso.max || ev_settings_set->iso_norm_max < cam_settings_limits.iso.min)
	{
		errors->set_ev_settings_value_error = SET;
		return RESET;
	}
	
	ev_settings.ev_correction		= ev_settings_set->ev_correction;
	ev_settings.f_norm					= ev_settings_set->f_norm;
	ev_settings.f_norm_min			= ev_settings_set->f_norm_min;
	ev_settings.iso_norm_max		= ev_settings_set->iso_norm_max;
	
	return SET;
}

//================================================================
static uint8_t find_optimal_parametr(int8_t * value, int8_t step, int8_t limit, ev_math_cam_settings_t * cam_settings, float ev, float delta)
{
	if (!step) return RESET;
	
	volatile float cam_ev;
	
	if (step > 0)
	{
		while (*value < limit)
		{
			*value = *value + step;
			cam_ev = ev_calc(cam_settings);
			if (compare(ev, cam_ev, delta)) return SET;
		}
	}
	else
	{
		while (*value > limit)
		{
			*value = *value + step;
			cam_ev = ev_calc(cam_settings);
			if (compare(ev, cam_ev, delta)) return SET;
		}
	}
	return RESET;
}

//================================================================
static float ev_calc(ev_math_cam_settings_t *cam_settings)
{
	return ((float)cam_settings->f / cam_settings_limits.f.denom + (float)cam_settings->s / cam_settings_limits.s.denom - (float)cam_settings->iso / cam_settings_limits.iso.denom);
}

//================================================================ 
static uint8_t compare(float val_1, float val_2, float delta)
{
	if (fabs(val_1 - val_2) < delta) return SET;
	return RESET;
}

//================================================================
static void reset_errors(ev_math_errors_t *errors)
{
	errors->module_no_init							= RESET;
	errors->set_ev_settings_value_error	= RESET;
	errors->too_ligth										= RESET;
	errors->too_dark										= RESET;
}

