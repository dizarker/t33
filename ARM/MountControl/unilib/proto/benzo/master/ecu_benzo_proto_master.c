#include "include.h"

/*
	Test sequence
	
	static uint32_t state_change = 0;
	
	state_change++;
	
	switch(state_change)
	{
		case 1:
			benzo_proto_data.out.default_needed_pressure = 1000;
			benzo_proto_data.out.needed_pressure		= 1300;
			break;
		case 60:
			benzo_proto_data.out.default_needed_pressure = 1000;
			benzo_proto_data.out.needed_pressure		= 1500;
			break;
		case 120:
			benzo_proto_data.out.default_needed_pressure = 1300;
			benzo_proto_data.out.needed_pressure		= 2000;
			break;
		case 180:
			benzo_proto_data.out.default_needed_pressure = 1300;
			benzo_proto_data.out.needed_pressure		= 2500;
			break;
		case 240:
			benzo_proto_data.out.default_needed_pressure = 1150;
			benzo_proto_data.out.needed_pressure		= 1400;
			break;
		default:
			break;		
	}
*/

//=============================================================================
void ecu_benzo_proto_master_2Hz(void)
{
	ecu_benzo_proto_frame_header_t header = 
	{
		.packet_type = 1
	};	
	
	ecu_benzo_proto_tx(&ecu_benzo_proto_handle, &header, &benzo_proto_data.out, sizeof(benzo_proto_data.out));

}
