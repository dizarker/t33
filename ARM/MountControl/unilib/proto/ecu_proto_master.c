#include "include.h"




//=============================================================================
void ecu_proto_master_task(void)
{

}


//=============================================================================
void ecu_proto_master_tx_info_pckt(void)
{
	ecu_proto_frame_header_t header =
	{
		.all_size 		= sizeof(ecu_service_info) + (ECU_PROTO_HEADER_SIZE - ECU_PROTO_PREFIX_SIZE),
		.packet_type 	= TYPE_PCKT1_DEVICE_INFO,
		.subtype		= 1
	};
	
	ecu_proto_master_tx(&ecu_proto_handle, &header, &ecu_service_info);
}
