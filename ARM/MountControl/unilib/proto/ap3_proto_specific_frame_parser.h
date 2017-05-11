/* 
 * ap3 protocol frame parser rev:0.1.8 [17.10.2011]
 *
 */


#ifndef _AP3_PROTO_FRAME_PARSER_H_
#define _AP3_PROTO_FRAME_PARSER_H_

enum
{
	PACKET_TYPE5_LOGGER_DATA 		= 5,
	PACKET_TYPE6_GET_STATUS_PCKT 	= 6
};

void ap3_proto_frame_parser(ap3_proto_frame_t* frame);


#endif
