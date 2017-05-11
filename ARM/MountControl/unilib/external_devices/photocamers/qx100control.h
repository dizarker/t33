#ifndef _QX100CONTROL_H_
#define _QX100CONTROL_H_

typedef struct
{	
	uint8_t state;	
	uint8_t off;
	uint8_t already_on;
	uint8_t need_state;
	float atribute_onoff_dist_to_line; //this attribute use for auto on/off before shooting the point. 
}qx100_control_t;


enum  // STATES OFF
{
	NOT       	= 0,
	TEMPORARILY = 1,
	FOREVER_OFF	= 2	
};

enum  // LIMIT FLY PARAM WORK QX100
{
	LIMIT_SPEED_KM_H = 30,
	LIMIT_ALT_ON	 = 50,
	LIMIT_ALT_OFF	 = 40
};



void qx100_init(void);
void qx100_zoom_test(void);
void qx100_task(void);

void qx100_cmd_on 	(qx100_control_t *qx_cntrl);
void qx100_cmd_off	(qx100_control_t *qx_cntrl, uint32_t off_sense);

uint32_t 	qx100_set_onoff_manual	(uint_fast16_t pac_number, uint8_t* data, uint8_t size);


extern qx100_control_t qx100_control;
extern timeout_t qx100_on_off_timer;
extern timeout_t qx100_off_after_test_photo_timer;
extern timeout_t qx100_repeat_onoff_timer;
extern uint32_t qx100_sleep_tics_start;
//extern timeout_t qx100_off_timer;

#endif
