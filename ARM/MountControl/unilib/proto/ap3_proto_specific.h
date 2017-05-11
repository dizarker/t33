/* 
 * ap3 protocol rev:0.1.8 [05.10.2011]
 *
 *  Serial port cfg: 115200-8-n-1
 *
 *     Fields:                     type:    size:
 *  1. Prefix - 0xB0 0x3B 0x4F     uint8   3 byte
 *  2. Data size                   uint8   1 byte  (w/o prefix and crc)
 *  3. NOT Data size (~)           uint8   1 byte
 *  4. Talker ID  (big-endian)     uint16  2 bytes
 *  5. Packet type                 uint8   1 byte
 *  6. Data + CRC8                 uint8   [0..250] data bytes + 1 byte crc
 *
 */


#ifndef _AP3_PROTO_H_
#define _AP3_PROTO_H_

#define AP3_PROTO_UART               uart_log

#define AP3_PROTO_PREFIX_STR         "\xB0\x3B\x50"
#define AP3_PROTO_PREFIX_SIZE        (sizeof(AP3_PROTO_PREFIX_STR)-1)


#define AP3_PROTO_HEADER_SIZE        sizeof(ap3_proto_frame_header_t)
#define AP3_PROTO_MAX_DATA_SIZE      450
#define AP3_PROTO_CRC_SIZE           1


enum
{
	TALKER_ID_NONE      = 0,
	TALKER_ID_AUTOPILOT = 1,
	TALKER_ID_MAGNITOMETR = 77
};

#pragma pack(push, 1)

typedef	struct
{                                             // offset  size
	uint8_t  prefix[AP3_PROTO_PREFIX_SIZE];   // 0       3
	uint8_t  all_size;                        // 3       1
	uint8_t  not_all_size;                    // 4       1
	uint16_t talker_id_bendian;               // 5       2
	uint8_t  packet_type;                     // 7       1
} ap3_proto_frame_header_t;


typedef	struct
{                                           
	ap3_proto_frame_header_t header;       
	uint8_t                  data[AP3_PROTO_MAX_DATA_SIZE+AP3_PROTO_CRC_SIZE]; // + 1 byte crc8
} ap3_proto_frame_t;

#pragma pack(pop) // default

typedef struct
{
	uint8_t  buf[sizeof(ap3_proto_frame_t)];
	int      idx;

	int      state;

	uint8_t* resync_buf;
	int      resync_qty;

	uart_t*  uart;
	
	int update;

} ap3_proto_handle_t;


void ap3_proto_init       (ap3_proto_handle_t* h);
void ap3_proto_slave_task (ap3_proto_handle_t* h);


#endif
