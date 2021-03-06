/* 
 * ecu protocol frame parser rev:
 *
 */


#ifndef _ECU_PROTO_SLAVE_H_
#define _ECU_PROTO_SLAVE_H_

enum
{
	TYPE_PCKT1_DEVICE_INFO 				= 1,
	TYPE_PCKT2_CMD_CONTROL				= 2,
	TYPE_PCKT3_DEBUG_TELEMETRY			= 3,
	TYPE_PCKT4_TIME_PROFILER			= 4,
	TYPE_PCKT5_STNGS_ECU_INJ			= 5,
	TYPE_PCKT6_MODE_EXCHNG_W_PC			= 6,	
	TYPE_PCKT7_STNGS_EXCHNG_W_LOGGER	= 7,
	TYPE_PCKT8_STEP_BY_STEP_TESTING		= 8
};

enum
{
	SUBTYPE_PCKT2_ENGINE_START				= 1,
	SUBTYPE_PCKT2_ENGINE_STOP				= 2,
	SUBTYPE_PCKT2_SET_PRECENT_OF_GAUSE		= 3,
	SUBTYPE_PCKT2_RESET_ENGINE_ERROR		= 4,
	SUBTYPE_PCKT2_READ_DATA_FROM_EEPROM		= 5,	
	SUBTYPE_PCKT2_SET_EVEN_ODD_INJECTION	= 6,
	
	SUBTYPE_PCKT3_DATA_OUT_1kHZ				= 1,
	SUBTYPE_PCKT3_DATA_OUT_50HZ				= 2,
	SUBTYPE_PCKT3_DATA_OUT_2HZ				= 3,
	SUBTYPE_PCKT3_DATA_OUT_ERROR_ENGINE		= 4,
	
	SUBTYPE_PCKT4_TABLE_TIME_MARKER			= 1,
	
	SUBTYPE_PCKT5_ECU_SETTINGS				= 1,
	
	SUBTYPE_PCKT6_NORMAL_MODE				= 1,
	SUBTYPE_PCKT6_KTR_MODE					= 2,
	
	SUBTYPE_PCKT7_LOGGER_SLOW_MODE			= 1,
	SUBTYPE_PCKT7_LOGGER_QUERY_INFO			= 2
	
};

void ecu_proto_frame_parser(ecu_proto_handle_t* h);
void ecu_proto_slave_reply(ecu_proto_handle_t* h, size_t size);
#endif
