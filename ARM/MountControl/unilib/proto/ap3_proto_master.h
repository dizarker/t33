
#ifndef _AP3_PROTO_MASTER_H_
#define _AP3_PROTO_MASTER_H_

#pragma pack(push, 1)
typedef struct
{
	//uint8_t time_fly;
	uint32_t all_size_SD;
	uint32_t bytes_written;
}ap3_proto_pckt_type6_t;

#pragma pack(pop) // default


void ap3_proto_master_tx(ap3_proto_frame_t* frame, ap3_proto_pckt_type6_t* answer);

#endif
