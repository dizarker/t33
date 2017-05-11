
#ifndef _ECU_PROTO_MASTER_H_
#define _ECU_PROTO_MASTER_H_


void ecu_proto_master_tx(ecu_proto_handle_t* h, ecu_proto_frame_header_t *header, const void* data);
void ecu_proto_master_tx_info_pckt(void);
#endif
