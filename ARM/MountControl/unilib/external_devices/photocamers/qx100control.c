#include "include.h"

/*
* qx100 camera control
* Author: 	Pluzhnikov E.
* Last rev: 2015.03.03
* 
*/

#ifdef SONY_QX_100

uint32_t qx100_sleep_tics_start;

//timeout_t qx100_off_timer;
timeout_t qx100_repeat_onoff_timer;
timeout_t qx100_off_after_test_photo_timer;
timeout_t qx100_blck_test_photo_timer;
timeout_t qx100_onoff;



qx100_control_t qx100_control;

static void qx100_get_state(void);
static void qx100_on_off(void);
static void qx100_forced_reload(void);


uint32_t qx100_sleep_tics_start = 0xffffff;

//=============================================================================
void qx100_zoom_test(void)
{

}

//=============================================================================
static void qx100_on_off(void)
{
	pin_set(PIN_QX100_ON_OFF);
	delay_ms(200);
	pin_clr(PIN_QX100_ON_OFF); //during this time qx100 cant photo and other function of afs stop	
}

//=============================================================================
void qx100_task(void)
{	
	qx100_get_state();
	
	//выкл. после тестового кадра, если нет данных по ап3
	if(!ap3_proto_handle.data_rx)
	{
		if(software_timer(&qx100_off_after_test_photo_timer)) 
		{						
			qx100_off_after_test_photo_timer.stop 			= 1;
			qx100_cmd_off(&qx100_control, TEMPORARILY);
			//dprintf("off after testphoto\n");
		}
	}
	
#ifndef ARCTICA	
	if(ap3_proto_handle.data_rx)
	{					
		static uint32_t fly_flag = 0;
		
		qx100_off_after_test_photo_timer.stop 			= 1;
		
		//режим полет
		if(ap3_data.air_speed > LIMIT_SPEED_KM_H && ap3_data.air_alt > LIMIT_ALT_ON)
		{ 
			fly_flag = 1;
			//dprintf("Rgm polet: air_alt = %f, air_speed = %d\n", ap3_data.air_alt, ap3_data.air_speed);
		}
		
		//режим набор, выкл. если начали приходить данные от ап3 и параметры системы меньше указанных
		if(ap3_data.air_speed < LIMIT_SPEED_KM_H && ap3_data.air_alt < LIMIT_ALT_ON)
		{
			qx100_cmd_off(&qx100_control, TEMPORARILY);
			//dprintf("Rgm nabor: air_alt = %f, air_speed = %d, flyflag = %d\n", ap3_data.air_alt, ap3_data.air_speed, fly_flag);
		}
		
		// выкл. навсегда: если произошли события парашют, буфер, земля, барометрическая высота менее 50м. ТОЛЬКО ДЛЯ ЛЕНОБЛ.													
		if( (ap3_data.air_alt < LIMIT_ALT_OFF  && fly_flag) | (ap3_data.event & 14))
		{							
			qx100_cmd_off(&qx100_control, FOREVER_OFF);
			//dprintf("Rgm forever off:air_alt = %f, flyflag = %d, event = %d\n", ap3_data.air_alt, fly_flag, (ap3_data.event & 14));			
		}
	}
#endif
	
	qx100_forced_reload();
	
	if( qx100_control.state != qx100_control.need_state)
	{
		//real time onoff qx100 is 10 sec, so its delay waiting, when the real state_onoff change
		if(software_timer(&qx100_repeat_onoff_timer))
		{
			qx100_on_off();
			//dprintf("onoff\n");
		}
	}
	else
	{
		//first action(on or off) make without delay, than timer on delay 10 sec.
		qx100_repeat_onoff_timer.update_end_sense 	= 0;
	}
}

//=============================================================================
uint32_t qx100_set_onoff_manual(uint_fast16_t pac_number, uint8_t* data, uint8_t size)
{
	#define NEED_STATE_OFFSET		(2)
	
	uint_fast8_t need_state = data[NEED_STATE_OFFSET];
			
	static uint_fast16_t pac_number_prev = (uint_fast16_t)-1;
	
	if (pac_number==pac_number_prev)
	{		
		return PAK_ERR_DUP_PACK;
	}

	if(need_state)
	{
		qx100_cmd_on(&qx100_control);
		//dprintf("on manual\n");
	}
	else
	{
		qx100_cmd_off(&qx100_control, TEMPORARILY);
		//dprintf("off manual tempo\n");
	}

	pac_number_prev = pac_number;

	return 0;
}

//=============================================================================
static void qx100_get_state(void)
{		
	if( (btn_photo.state | pin_test(PIN_PHOTO))
		| photo.picture.awaiting 
		| photo.picture.done 
		) 
	{
		qx100_control.state = 1;
		//dprintf("on\n");
	}
	else if(!pin_test(PIN_PHOTO))
	{
		qx100_control.state = 0;
		//dprintf("off\n");
	}
	//dprintf("state qx100 = %d\n", qx100_control.state);
}

//=============================================================================
void qx100_cmd_off(qx100_control_t *qx_cntrl, uint32_t off_sense)
{
	qx_cntrl->need_state	= 0;
	qx_cntrl->off 			= off_sense;
}

//=============================================================================
void qx100_cmd_on(qx100_control_t *qx_cntrl)
{
	if(qx_cntrl->off != FOREVER_OFF)
	{	
		qx_cntrl->off 			= NOT;
		qx_cntrl->need_state	= 1;
	}
}

//=============================================================================
static void qx100_forced_reload(void)
{
	/*
	static const int time_until_autoreload 	= 2 * 60; //120 sec after this time inactive qx100 enter in sleep mode.
	static const int time_reload			= 25; 	  								//sec
	static const int time_autoreload		= TOTICS((time_until_autoreload - time_reload) * 1000); 	
			
	uint32_t time_elapsed_tics = (systick_timer.tics - qx100_sleep_tics_start);

	
	static int i = 0;
	
	if(++i == 50)
	{		
		dprintf("dist 	= %f\n", qx100_control.atribute_onoff_dist_to_line);
		i = 0;
	}
	if(time_elapsed_tics >= time_autoreload)
	{	}
	*/
	
	static int already_do = 0;
	if( qx100_control.atribute_onoff_dist_to_line < 300 && !already_do )
	{
		//dprintf("i am here\n");
		//qx100_sleep_tics_start = systick_timer.tics;
		if(qx100_control.need_state == 1) qx100_on_off();
		already_do = 1;
	}
	else if(qx100_control.atribute_onoff_dist_to_line > 300)
	{
		already_do = 0;
	}

}


//=============================================================================
void qx100_init(void)
{	
	qx100_control.state 					= 0;
	qx100_control.off						= TEMPORARILY;
	qx100_control.atribute_onoff_dist_to_line	= 350;	
	
	qx100_off_after_test_photo_timer.ms 				= 60000;
	qx100_off_after_test_photo_timer.update_end_sense 	= 0;
	qx100_off_after_test_photo_timer.stop				= 1;
	
	qx100_blck_test_photo_timer.ms 				= 2000;
	qx100_blck_test_photo_timer.update_end_sense 	= 0;
	qx100_blck_test_photo_timer.stop				= 0;
	
	qx100_repeat_onoff_timer.ms					= 10000;
	qx100_repeat_onoff_timer.update_end_sense 	= 0;
	qx100_repeat_onoff_timer.stop				= 0;
	
	
	
	//this delay use for correct definition the state of qx100 after power on. 
	delay_sec(10);
}	

#endif

