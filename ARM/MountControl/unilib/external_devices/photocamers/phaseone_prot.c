//PHASEONE iX Link Protocol 1.1

#include "include.h"

//****************************************************************
//structs_t
//****************************************************************

#pragma pack(push,1)

typedef struct
{
	int8_t prefix;
	int8_t message_size; 
	int8_t protocol_version;
	uint8_t command_id;
	int8_t msg[3];
} cam_request_t;

typedef struct
{
	int8_t prefix;
	int8_t message_size;
	int8_t protocol_version;
	int8_t command_completion_code;
	uint8_t command_id;
	uint8_t msg[10];
} cam_reply_t;

#pragma pack(pop)

//****************************************************************
//prototypes of functions
//****************************************************************

uint8_t phaseone_prot_task(phaseone_prot_errors_t *errors);
uint8_t phaseone_prot_init(phaseone_prot_errors_t *errors);
uint8_t phaseone_prot_set_ev(float ev, phaseone_prot_errors_t *errors);
uint8_t phaseone_prot_set_ev_settings(ev_math_ev_settings_t *ev_settings_set, phaseone_prot_errors_t *errors);

static uint8_t check_cam_setting_set(ev_math_cam_settings_t *fl_cam_settings);
static void reset_errors(phaseone_prot_errors_t *errors);
static void set_settings(ev_math_cam_settings_t * cam_settings, phaseone_prot_errors_t *errors);
static uint8_t fill_settings_limits(int8_t command_id, ev_math_cam_settings_group_t * settings_group, phaseone_prot_errors_t *errors);
static void send_set_request(int8_t command_id, int8_t value_num, int8_t value_denom);
static void send_get_request(int8_t command_id);
static uint8_t get_reply(int8_t command_id, phaseone_prot_errors_t *errors);
static uint8_t check_reply(void);
static int xor_checksum(int8_t *s, int cnt) ;

//****************************************************************
//module variables
//****************************************************************

static char 													fl_module_init = RESET;
static cam_request_t									cam_request = {88, 0, 1, 0};
static cam_reply_t 										cam_reply;
static ev_math_cam_settings_limits_t	cam_settings_limits;
static ev_math_cam_settings_t					fl_cam_settings_set_no_confirm;
static ev_math_cam_settings_t 				cam_settings = {RESET, RESET, RESET};
static timeout_t 											timeout_replay_set_request;

//****************************************************************
//functions
//****************************************************************

//================================================================
uint8_t phaseone_prot_init(phaseone_prot_errors_t *errors)
{		
	reset_errors(errors);
	
	uart_init(&PHASEONE_UART, PO_PROTOCOL_BAUDRATE);
	
	if (!fill_settings_limits(PO_CM_GET_F_RANGE, 		&cam_settings_limits.f,		errors) ||
			!fill_settings_limits(PO_CM_GET_S_RANGE, 		&cam_settings_limits.s,		errors) ||
			!fill_settings_limits(PO_CM_GET_ISO_RANGE,	&cam_settings_limits.iso,	errors))
	{
		errors->module_no_init = SET;
		fl_module_init = RESET;
		return RESET;
	}
	
	if (!ev_math_init(&cam_settings_limits))
	{
		errors->module_no_init = SET;
		errors->camera_settings_value_error = SET;
		fl_module_init = RESET;
		return RESET;
	}
	
	fl_module_init = SET;
	return SET;
}

//================================================================
uint8_t phaseone_prot_task(phaseone_prot_errors_t *errors)
{
	static int8_t cam_settings_set_try_cnt = 1;
	reset_errors(errors);
	
	if (check_cam_setting_set(&fl_cam_settings_set_no_confirm)) 
	{
		cam_settings_set_try_cnt = 1;
		return SET;
	}
	
	while (check_reply())
	{
		if (cam_reply.command_completion_code != 0)
		{
			errors->fl_camera_msg_error = SET;
			errors->camera_msg_error.resend_command_competition_code = cam_reply.command_completion_code;
			errors->camera_msg_error.send_command_id = cam_reply.command_id;
			
			if(cam_reply.command_id == PO_CM_SET_F)
			{
				errors->camera_msg_error.send_value_num = cam_settings.f;
				errors->camera_msg_error.send_value_denom = cam_settings_limits.f.denom;
			}
			else if (cam_reply.command_id == PO_CM_SET_S)
			{
				errors->camera_msg_error.send_value_num = cam_settings.s;
				errors->camera_msg_error.send_value_denom = cam_settings_limits.s.denom;
			}
			else if (cam_reply.command_id == PO_CM_SET_ISO)
			{
				errors->camera_msg_error.send_value_num = cam_settings.iso;
				errors->camera_msg_error.send_value_denom = cam_settings_limits.iso.denom;
			}	
			return RESET;
		}
		
		if(cam_reply.command_id == PO_CM_SET_F) fl_cam_settings_set_no_confirm.f = RESET;
		else if (cam_reply.command_id == PO_CM_SET_S) fl_cam_settings_set_no_confirm.s = RESET;
		else if (cam_reply.command_id == PO_CM_SET_ISO)  fl_cam_settings_set_no_confirm.iso = RESET;
	}
	
	if (software_timer(&timeout_replay_set_request))
	{
		cam_settings_set_try_cnt++;
		if (cam_settings_set_try_cnt > PO_REQUEST_SEND_TRY_CNT)
		{
			fl_cam_settings_set_no_confirm.f = RESET;
			fl_cam_settings_set_no_confirm.s = RESET;
			fl_cam_settings_set_no_confirm.iso = RESET;
			
			errors->camera_no_response = SET;
			return RESET;
		}
		set_settings(&cam_settings, errors);
	}
	
	return SET;
}

//================================================================
uint8_t phaseone_prot_set_ev(float ev, phaseone_prot_errors_t *errors)
{
	ev_math_errors_t ev_math_errors;
	
	reset_errors(errors);
	if (!fl_module_init)
	{
		errors->module_no_init = SET;
		return RESET;
	}
	
	if (!check_cam_setting_set(&fl_cam_settings_set_no_confirm))
	{
		errors->camera_set_settings_busy = SET;
		return RESET;
	}
		
	if (ev_math_calc_cam_settings(ev, &cam_settings, &ev_math_errors))
	{
		set_settings(&cam_settings, errors);
		return SET;
	}
	else
	{
		if (ev_math_errors.module_no_init)
		{
			fl_module_init = RESET;
			errors->module_no_init = SET;
			return RESET;
		}
		
		errors->too_dark = ev_math_errors.too_dark;
		errors->too_ligth = ev_math_errors.too_ligth;
		
		set_settings(&cam_settings, errors);
		
		return RESET;
	}
}

//================================================================
uint8_t phaseone_prot_set_ev_settings(ev_math_ev_settings_t *ev_settings_set, phaseone_prot_errors_t *errors)
{
	ev_math_errors_t ev_math_errors;
	
	reset_errors(errors);
	if (!fl_module_init)
	{
		errors->module_no_init = SET;
		return RESET;
	}
	
	if (ev_math_set_ev_settings(ev_settings_set, &ev_math_errors))
	{
		return SET;
	}
	else
	{
		if (ev_math_errors.module_no_init)
		{
			fl_module_init = RESET;
			errors->module_no_init = SET;
			return RESET;
		}
		errors->set_ev_settings_value_error = ev_math_errors.set_ev_settings_value_error;
		return RESET;
	}
}

//================================================================
static uint8_t check_cam_setting_set(ev_math_cam_settings_t *fl_cam_settings)
{
	if (!fl_cam_settings->f && !fl_cam_settings->s && !fl_cam_settings->iso)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

//================================================================
static void reset_errors(phaseone_prot_errors_t *errors)
{
	errors->camera_msg_error.resend_command_competition_code	=	RESET;
	errors->camera_msg_error.send_command_id									=	RESET;
	errors->camera_msg_error.send_value_denom									=	RESET;
	errors->camera_msg_error.send_value_num										=	RESET;
	errors->module_no_init																		= RESET;
	errors->camera_no_response																= RESET;
	errors->camera_internal_error															= RESET;
	errors->camera_settings_value_error												= RESET;
	errors->set_ev_settings_value_error												= RESET;
	errors->too_ligth																					= RESET;
	errors->too_dark																					= RESET;
	errors->camera_set_settings_busy													= RESET;
}

//================================================================
static void set_settings(ev_math_cam_settings_t * cam_settings, phaseone_prot_errors_t *errors)
{		
	send_set_request(PO_CM_SET_F, cam_settings->f, cam_settings_limits.f.denom);
	fl_cam_settings_set_no_confirm.f = SET;
	
	send_set_request(PO_CM_SET_S, cam_settings->s, cam_settings_limits.s.denom);	
	fl_cam_settings_set_no_confirm.s = SET;
	
	send_set_request(PO_CM_SET_ISO, cam_settings->iso, cam_settings_limits.iso.denom);
	fl_cam_settings_set_no_confirm.iso = SET;
	
	timeout_replay_set_request.ms = PO_REPLAY_SET_REQUEST_WAIT_MS;
	timeout_replay_set_request.stop = 0;
	timeout_replay_set_request.update_end_sense = 1;
}

//================================================================
static uint8_t fill_settings_limits(int8_t command_id, ev_math_cam_settings_group_t * settings_group, phaseone_prot_errors_t *errors)
{		
	if (!get_reply(command_id, errors)) return RESET;
	
	if (cam_reply.command_completion_code != 0)
	{
		errors->fl_camera_msg_error = SET;
		errors->camera_msg_error.resend_command_competition_code = cam_reply.command_completion_code;
		errors->camera_msg_error.send_command_id = command_id;
		
		return RESET;
	}
	
	settings_group->min		= cam_reply.msg[0];
	settings_group->max		= cam_reply.msg[1];
	settings_group->step	= cam_reply.msg[2];
	settings_group->denom	= cam_reply.msg[3];
	return SET;
}

//================================================================
static void send_set_request(int8_t command_id, int8_t value_num, int8_t value_denom)
{	
	cam_request.message_size = 4;
	cam_request.protocol_version = PO_PROTOCOL_VERSION;
	cam_request.command_id = command_id;
	cam_request.msg[0] = value_num;
	cam_request.msg[1] = value_denom;
	cam_request.msg[2] = xor_checksum((int8_t*)&cam_request + 2, cam_request.message_size);
	
	uart_put_block(&PHASEONE_UART, &cam_request, cam_request.message_size + 3);
}

//================================================================
static void send_get_request(int8_t command_id)
{	
	cam_request.message_size = 2;
	cam_request.protocol_version = PO_PROTOCOL_VERSION;
	cam_request.command_id = command_id;
	cam_request.msg[0] = xor_checksum((int8_t*)&cam_request + 2, cam_request.message_size);

	uart_put_block(&PHASEONE_UART, &cam_request, cam_request.message_size + 3);
}

//================================================================
static uint8_t get_reply(int8_t command_id, phaseone_prot_errors_t *errors)
{
	int8_t fl_no_data  = PO_REQUEST_SEND_TRY_CNT;
	uint32_t delay_cnt = 0;
	
	send_get_request(command_id);
	while (fl_no_data)
	{	
		if (check_reply()) break;
		
		delay_ms(1);
		delay_cnt++;
		
		if (delay_cnt > PO_REPLAY_GET_REQUEST_WAIT_MS) 
		{
			
			delay_cnt = 0;
			
			fl_no_data--;
			
			if (!fl_no_data)
			{
				errors->camera_no_response = SET;
				return RESET;
			}
			send_get_request(command_id);
		}
	}
	return SET;
}

//================================================================
static uint8_t check_reply(void)
{	
	int32_t rx_tmp;
	uint8_t rx;
	static uint8_t cnt = 0;
	
	while ((rx_tmp = uart_getc(&PHASEONE_UART)) != -1)
	{
		rx = rx_tmp;
		
		if (cnt == 0) //prefix
		{
			if (rx == 88) cnt++;
		}
		else if (cnt == 1) //message_size
		{
			cam_reply.message_size = rx;
			if (cam_reply.message_size > 8) cnt = 0; //проверка на адекватность
			else cnt++;
		}
		else //message
		{
			*((uint8_t*)&cam_reply + cnt) = rx;
			cnt++;
			if (cnt > (cam_reply.message_size + 2))
			{
				cnt = 0;
				if (cam_reply.msg[cam_reply.message_size - 3] != (uint8_t)xor_checksum((int8_t*)&cam_reply + 2, cam_reply.message_size)) return RESET;
				return SET;
			}
		}
	}
	return RESET;
}

//================================================================
static int xor_checksum(int8_t *s, int cnt) 
{
    int c = 0;
    while(cnt --) c ^= *s++;
    return c;
}
