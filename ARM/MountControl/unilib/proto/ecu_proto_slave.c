#include "include.h"


//=============================================================================
void ecu_proto_slave_reply(ecu_proto_handle_t* h, size_t size)
{	
	uart_put_block(h->uart, &h->buf_rx[0], size);
}

//=========================================================================
void ecu_proto_frame_parser(ecu_proto_handle_t* h)
{		
	pin_toggle(PIN_LED_GREEN_EXCHANGE);
	
	if(TYPE_PCKT2_CMD_CONTROL == h->header.packet_type)
	{	
		if(SUBTYPE_PCKT2_ENGINE_START == h->header.subtype)
		{
			//dprintf("SUBTYPE_PCKT2_ENGINE_START\n");
		}		
		
		if(SUBTYPE_PCKT2_ENGINE_STOP == h->header.subtype)
		{
			//dprintf("SUBTYPE_PCKT2_ENGINE_STOP\n");		
		}
		if(SUBTYPE_PCKT2_SET_PRECENT_OF_GAUSE == h->header.subtype)
		{
			//dprintf("SUBTYPE_PCKT2_SET_PRECENT_OF_GAUSE\n");			
		}
		if(SUBTYPE_PCKT2_RESET_ENGINE_ERROR == h->header.subtype)
		{
			
		}
		
		if(SUBTYPE_PCKT2_READ_DATA_FROM_EEPROM == h->header.subtype)
		{
			//dprintf("SUBTYPE_PCKT2_SET_PRECENT_OF_GAUSE\n");			
		}
		if(SUBTYPE_PCKT2_SET_EVEN_ODD_INJECTION == h->header.subtype)
		{
			
		}
					
		ecu_proto_slave_reply(h, (ECU_PROTO_PREFIX_SIZE + ECU_PROTO_CRC_SIZE + h->header.all_size));
	}

	if(TYPE_PCKT5_STNGS_ECU_INJ == h->header.packet_type)				
	{	
		if(SUBTYPE_PCKT5_ECU_SETTINGS == h->header.subtype)
		{
			
		}
		ecu_proto_slave_reply(h, (ECU_PROTO_PREFIX_SIZE + ECU_PROTO_CRC_SIZE + h->header.all_size));
	}
	if(TYPE_PCKT6_MODE_EXCHNG_W_PC == h->header.packet_type)
	{
		if(SUBTYPE_PCKT6_NORMAL_MODE == h->header.subtype)
		{
			ecu.settings_proto.normal_mode_on = 1;
			//set max speed com port = 921600;
			ecu.settings_proto.freq_out[0].divider 	= 1;
			ecu.settings_proto.freq_out[1].divider 	= 1;
			ecu.settings_proto.freq_out[2].divider 	= 1;
		}
		if(SUBTYPE_PCKT6_KTR_MODE == h->header.subtype)//temporary mode 
		{
			ecu.settings_proto.normal_mode_on = 0;
			ecu.settings_proto.freq_out[0].divider 	= DATA_OUT_1Hz;
			ecu.settings_proto.freq_out[1].divider 	= DATA_OUT_05Hz;
			ecu.settings_proto.freq_out[2].divider 	= DATA_OUT_02Hz;
			//set speed com port = 115200;
		}
	}

	if(TYPE_PCKT7_STNGS_EXCHNG_W_LOGGER == h->header.packet_type) 
	{
		if(SUBTYPE_PCKT7_LOGGER_SLOW_MODE == h->header.subtype) 
		{
			//set freq out to logger slow.
		}
	}
/* 
*	NOT USED
*	if(TYPE_PCKT8_STEP_BY_STEP_TESTING == h->header.packet_type)
*	{		
*		//debug_mode_on();
*	}
*/
		
}
