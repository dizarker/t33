#ifndef _ECU_BENZO_PROTO_H_
#define _ECU_BENZO_PROTO_H_

/* 
 * ecu_benzo protocol rev:0.0.1 [07.10.2015]
 *	1 wire proto
 *  Serial port cfg: 115200-8-n-1
 *
 *     Fields:                     	type:    size:
 *  1. Prefix - 0xB0 0x3B 0x4F     	uint8   3 byte 
 *	2. Type pckt					uint8	1 byte	
 *  3. Data			               	uint8   [0..64]
 *
 */

#define ECU_BENZO_PROTO_UART               uart_logger

#define ECU_BENZO_PROTO_PREFIX_STR         "\xB0\x3B\x58"
#define ECU_BENZO_PROTO_PREFIX_SIZE        (sizeof(ECU_BENZO_PROTO_PREFIX_STR)-1)
#define ECU_BENZO_PROTO_BODY_SIZE			3
#define ECU_BENZO_PROTO_CRC_SIZE			1


#define ECU_BENZO_PROTO_HEADER_SIZE     sizeof(ecu_benzo_proto_frame_header_t)
#define ECU_BENZO_PROTO_MAX_DATA_SIZE 	64

#define ECU_BENZO_SIZE_PREFIX			3
#define SIZE_PREFIX						ECU_BENZO_SIZE_PREFIX
#pragma pack(push, 1)

typedef	struct
{                                             		// offset  size
	uint8_t  prefix[ECU_BENZO_PROTO_PREFIX_SIZE];   // 0       3
	uint8_t  packet_type;  	                   		// 4       1			
} ecu_benzo_proto_frame_header_t;


typedef	struct
{                                           
	ecu_benzo_proto_frame_header_t header;
	uint8_t                  data[ECU_BENZO_PROTO_MAX_DATA_SIZE]; //
} ecu_benzo_proto_frame_t;

#pragma pack(pop) // default

typedef struct
{
	uint8_t  buf[sizeof(ecu_benzo_proto_frame_t)];
	int      idx;

	int      state;
	uart_t*  uart;
	
	int data_rx;
	int new_pckt;
	
} ecu_benzo_proto_handle_t;


void ecu_benzo_proto_init       (ecu_benzo_proto_handle_t* h);
void ecu_benzo_proto_slave_task (ecu_benzo_proto_handle_t* h);
void ecu_benzo_proto_tx(ecu_benzo_proto_handle_t* h, ecu_benzo_proto_frame_header_t* header, const void* data, size_t size);
#endif
