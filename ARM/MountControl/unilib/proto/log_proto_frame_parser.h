#ifndef _LOG_PROTO_FRAME_PARSER_H_
#define _LOG_PROTO_FRAME_PARSER_H_

enum 
{
	PACKET_TYPE3_PHOTO_DATA = 3,
	PACKET_TYPE4_ERROR_DATA = 4,
	PACKET_TYPE5_ARCTICA	= 5,
	PACKET_TYPE77_MAGNITOMETR	= 77
};





void log_proto_frame_parser(log_proto_frame_t* frame);


#endif
