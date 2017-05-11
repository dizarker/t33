
#ifndef _ECU_BENZO_FRAME_PROTO_H_
#define _ECU_BENZO_FRAME_PROTO_H_

enum
{
	PACKET_TYPE1_BENZO_DATA = 1
};
#pragma pack(push, 1)
typedef struct
{
	struct
	{
		uint16_t 	needed_pressure;
		uint16_t 	default_needed_pressure;
		uint8_t		fuel_lvl;
	}out;
	
	struct
	{
		uint16_t error;		
	}in;

}benzo_proto_data_t;
#pragma pack(pop) // default

void ecu_benzo_proto_frame_parser(ecu_benzo_proto_frame_t* frame);
#endif
