/* 
 * ecu protocol rev:0.1.8 [05.10.2011]
 *
 *  Serial port cfg: 115200-8-n-1(work with KTR) or max speed
 *
 *     Fields:						type:	size:
 *  1. Prefix - 0xB0 0x3B 0x51		uint8   3 byte
 *  2. Data size					uint8   2 byte  (w/o prefix and crc)
 *  3. NOT Data size (~)			uint8   2 byte
 *  4. Packet type					uint8   1 byte
 *  5. Subtype                		uint8   1 byte
 *  6. Data + CRC32					uint8   [0..250] data bytes + 4 byte crc
 *
 */


#ifndef _ECU_PROTO_H_
#define _ECU_PROTO_H_

#define ECU_PROTO_REV                     1
#define ECU_PROTO_REV_STR                "1.0"



#define ECU_PROTO_UART               uart_ecu

#define ECU_PROTO_PREFIX_STR         "\xB0\x3B\x51"
#define ECU_PROTO_PREFIX_SIZE        (sizeof(ECU_PROTO_PREFIX_STR)-1)


#define ECU_PROTO_HEADER_SIZE        sizeof(ecu_proto_frame_header_t)
#define ECU_PROTO_MAX_DATA_SIZE      500
#define ECU_PROTO_CRC_SIZE           4

/*
enum
{
	TALKER_ID_NONE      = 0,
};
*/

#pragma pack(push, 1)

typedef	struct
{                                             		// offset  size
	uint8_t		prefix[ECU_PROTO_PREFIX_SIZE];   	// 0       3
	uint16_t	all_size;                        	// 3       2
	uint16_t	not_all_size;                    	// 5       2
	uint8_t		packet_type;               			// 7       1
	uint8_t		subtype;                     		// 8       1
} ecu_proto_frame_header_t;

typedef struct
{
	ecu_proto_frame_header_t  header;
	
	uint8_t  buf_rx[ECU_PROTO_MAX_DATA_SIZE];
	uint8_t  buf_tx[ECU_PROTO_MAX_DATA_SIZE];
	int      idx;

	int      state;

	uint8_t* resync_buf;
	int      resync_qty;

	uart_t*  uart;
	
	int update;

} ecu_proto_handle_t;
#pragma pack(pop) // default

void ecu_proto_init(ecu_proto_handle_t* h);
void ecu_proto_task(ecu_proto_handle_t* h);

void ecu_proto_master_prepare_header(ecu_proto_handle_t* h, ecu_proto_frame_header_t *header);
void ecu_proto_master_prepare_crc(ecu_proto_handle_t* h, size_t size);
void ecu_proto_master_append_data(void *dst, const void* src, size_t size);
#endif
