#ifndef _LOG_PROTO_H_
#define _LOG_PROTO_H_

/* 
 * log protocol rev:0.0.1 [25.07.2014]
 *
 *  Serial port cfg: 115200-8-n-1
 *
 *     Fields:                     	type:    size:
 *  1. Prefix - 0xB0 0x3B 0x4F     	uint8   3 byte 
 *	2. Size							uint8	1 byte
	3. Type pckt					uint8	1 byte
 *  3. Data			               	uint8   [0..250] data bytes, at this moment 11 or 55 bytes data
 *
 */

#define LOG_PROTO_UART               uart_log

//#define LOG_PROTO_PREFIX_STR         "\xB0\x3B\x50" // magnitometr
#define LOG_PROTO_PREFIX_STR         "\xB0\x3B\x4f"	//ap3, afs_log
#define LOG_PROTO_PREFIX_SIZE        (sizeof(LOG_PROTO_PREFIX_STR)-1)


#define LOG_PROTO_HEADER_SIZE        sizeof(log_proto_frame_header_t)
#define LOG_PROTO_MAX_DATA_SIZE  500

#define ARCTICA_SIZE_PREFIX				3
#define SIZE_PREFIX						ARCTICA_SIZE_PREFIX
#pragma pack(push, 1)

typedef	struct
{                                             	// offset  size
	uint8_t  prefix[LOG_PROTO_PREFIX_SIZE];   	// 0       3
	uint8_t  all_size;  	                   	// 4       1		
	uint8_t  packet_type;                  		// 5       1
} log_proto_frame_header_t;


typedef	struct
{                                           
	log_proto_frame_header_t header;
	uint8_t                  data[LOG_PROTO_MAX_DATA_SIZE]; //
} log_proto_frame_t;

#pragma pack(pop) // default

typedef struct
{
	uint8_t  buf[sizeof(log_proto_frame_t)];
	int      idx;

	int      state;

	uint8_t* resync_buf;
	int      resync_qty;

	uart_t*  uart;
	
	int data_rx;
	int new_pckt;
	
} log_proto_handle_t;


void log_proto_init       (log_proto_handle_t* h);
void log_proto_slave_task (log_proto_handle_t* h);

#endif
