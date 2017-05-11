
#ifndef _PHASEONE_H_
#define _PHASEONE_H_

#define PO_REQUEST_SEND_TRY_CNT 3						//сколько раз пытаться посылать камере запрос
#define PO_REPLAY_GET_REQUEST_WAIT_MS 50		//сколько миллисекунд ждать ответ от камеры на запрос получения от неё параметров
#define PO_REPLAY_SET_REQUEST_WAIT_MS 7000	//сколько миллисекунд ждать ответ от камеры на запрос установки параметров

#define PO_PROTOCOL_VERSION 1
#define PO_PROTOCOL_BAUDRATE 9600

#define PO_CM_SET_F 10
#define PO_CM_GET_F 11
#define PO_CM_GET_F_RANGE 12

#define PO_CM_SET_S 18
#define PO_CM_GET_S  19
#define PO_CM_GET_S_RANGE 20

#define PO_CM_SET_ISO 14
#define PO_CM_GET_ISO 15
#define PO_CM_GET_ISO_RANGE 16

#define PO_GET_SYSTEM_STATUS 111

//================================================================
#pragma pack(push,1)
typedef struct
{
	int8_t send_command_id;
	int8_t send_value_num;
	int8_t send_value_denom;
	int8_t resend_command_competition_code;
} phaseone_msg_error_t;

typedef struct
{
	phaseone_msg_error_t camera_msg_error;	//расшифровка ошибки в ответе фотоаппарата
	int8_t fl_camera_msg_error;							//флаг наличия ошибки в ответе фотоаппарата
	int8_t module_no_init;									//модуль не инициализирован
	int8_t camera_no_response;							//нет ответа от камеры
	int8_t camera_internal_error;						//камера сообщила о внутренней ошибки
	int8_t camera_settings_value_error;			//модуль не может работать со значениями которые ему передала камера
	int8_t camera_set_settings_busy;				//камера занята установкой значений
	int8_t set_ev_settings_value_error;			//ошибка значений настройки экспозиции
	int8_t too_ligth;												//переизбыток света
	int8_t too_dark;												//недостаток света
} phaseone_prot_errors_t;
#pragma pack(pop)

//================================================================
//При отсутствии ошибок фунция возващает "1"
uint8_t phaseone_prot_init(phaseone_prot_errors_t *errors);
uint8_t phaseone_prot_task(phaseone_prot_errors_t *errors);
uint8_t phaseone_prot_set_ev(float ev, phaseone_prot_errors_t *errors);
uint8_t phaseone_prot_set_ev_settings(ev_math_ev_settings_t *ev_settings_set, phaseone_prot_errors_t *errors);

#endif
