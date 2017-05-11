#include "include.h"

#define LOG_PROTO_PREFIX_STR             "\xB0\x3B\x4F"
#define LOG_PROTO_PREFIX_SIZE            (sizeof(PAK_PROTO_PREFIX_STR)-1)

#pragma pack(push, 1)
typedef struct
{
	const uint8_t prefix[LOG_PROTO_PREFIX_SIZE];
	uint8_t size;
	uint8_t type_pckt;
}log_header_t;

typedef struct
{
	uint32_t utime;
	uint8_t state;
	uint8_t errors;
	uint8_t button_tries_qty;
	uint8_t in_reg_number;
	uint8_t qty_non_load_photo;
}error_info_t;

#pragma pack(pop)


#define LOG_FRAME_PHOTO_NEW_PICTURE_SIZE	(sizeof(pic_info_full_t) + 2)
#define LOG_FRAME_PHOTO_ERROR_SIZE			(sizeof(error_info_t) + 2)


static const int8_t LOG_FRAME_PHOTO_NEW_PICTURE = 3;
static const uint8_t LOG_FRAME_ERROR			= 4;

log_header_t log_header = 
{		
	.type_pckt = LOG_FRAME_PHOTO_NEW_PICTURE,
	.size = sizeof(pic_info_full_t) + 2,
};

error_info_t error_info;


//=============================================================================
void log_init(void)
{
	memcpy(&log_header, LOG_PROTO_PREFIX_STR, LOG_PROTO_PREFIX_SIZE);	
}

//=============================================================================
void log_photo_task(pic_info_full_t *pic_info, uart_t *uart)
{			
	memcpy(&log_header, LOG_PROTO_PREFIX_STR, LOG_PROTO_PREFIX_SIZE);
	
	log_header.size = LOG_FRAME_PHOTO_NEW_PICTURE_SIZE;
	log_header.type_pckt = LOG_FRAME_PHOTO_NEW_PICTURE;
	
	uart_put_block(uart, &log_header.prefix[0], sizeof(log_header_t));
	uart_put_block(uart, pic_info, sizeof(pic_info_full_t));
}

//=============================================================================
void log_error_task(uart_t *uart)
{
	
	log_header.size 				= LOG_FRAME_PHOTO_ERROR_SIZE;
	log_header.type_pckt 			= LOG_FRAME_ERROR;
	
	error_info.utime				= photo.status.info.utime;
	error_info.state				= photo.state;
	error_info.errors				= photo.status.errors;
	error_info.button_tries_qty 	= photo.button_tries_qty;
	error_info.in_reg_number		= photo.in_reg_number;
	error_info.qty_non_load_photo 	= photo.qty_non_load_photo;
		
	uart_put_block(uart, &log_header.prefix[0], sizeof(log_header_t));
	
	uart_put_block(uart, &error_info, sizeof(error_info_t));
}

//=============================================================================
void dlog_start(void)
{
	#define PREFIX_STR  "\xB0\x3B\x4F"

	dprint_binary(PREFIX_STR, sizeof(PREFIX_STR)-1); // prefix	
}

//=============================================================================
void dlog_append(const void* src, size_t qty)
{
	dprint_binary(src, qty);                         // raw binary data
}

