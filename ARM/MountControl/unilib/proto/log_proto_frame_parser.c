#include "include.h"


//=============================================================================
static void log_proto_packet_type3_parser(uint8_t* data, size_t size)
{
	//unixtime_to_UTC_str_task()
	write_in_file.qty_bytes = size + SIZE_PREFIX;
}

//=============================================================================
static void log_proto_packet_type4_parser(uint8_t* data, size_t size)
{
	#define LOG_PROTO_UTIME_OFFSET (5-5)
	utc_time.d32 = read_dword_lendian(&data[LOG_PROTO_UTIME_OFFSET]);
	unixtime_to_UTC_str_task(utc_time.d32, &utc_time);	
	write_in_file.qty_bytes = size + SIZE_PREFIX;
}

//=============================================================================
static void log_proto_packet_type5_parser(uint8_t* data, size_t size)
{
	#define LOG_PROTO_UTIME_OFFSET (5-5)	
	utc_time.d32 = read_dword_lendian(&data[LOG_PROTO_UTIME_OFFSET]);
	unixtime_to_UTC_str_task(utc_time.d32, &utc_time);
	write_in_file.qty_bytes = size + SIZE_PREFIX;
}

//=============================================================================
void log_proto_frame_parser(log_proto_frame_t* frame)
{
	switch (frame->header.packet_type)
	{
		case PACKET_TYPE3_PHOTO_DATA:
		{				
			log_proto_packet_type3_parser(frame->data, frame->header.all_size);
			break;
		}
		case PACKET_TYPE4_ERROR_DATA:
		{				
			log_proto_packet_type4_parser(frame->data, frame->header.all_size);
			break;
		}
		
		case PACKET_TYPE5_ARCTICA:
		{
			log_proto_packet_type5_parser(frame->data, frame->header.all_size);
			break;
		}
		
		default:
		{
			break;
		}
	}
}
