/* 
 * ap3 protocol frame parser rev:0.1.8 [17.10.2011]
 *
 */


#ifndef _AP3_PROTO_FRAME_PARSER_H_
#define _AP3_PROTO_FRAME_PARSER_H_

enum
{
	PACKET_TYPE5_LOGGER_DATA = 5
};

typedef struct
{
	float horizontal;   // horizontal velocity in m/s
	float lat;          // rad/s
	float lon;	        // rad/s
} speed_t;


typedef struct
{
	double lat;
	double lon;
}pos_t;

typedef struct
{
	pos_t pos_real;
	float alt;
	uint32_t utime;
	int32_t timeout;
	pos_t pos_proxi;
	pos_t pos_feedforward_offset;
	uint32_t update;
	speed_t  speed;
	float 	feedforward_delay_sec;
}ap3_estimator_t;


typedef struct
{
	uint8_t air_speed;
	uint8_t event;
	float air_alt;
	float gps_speed; 
	float gps_azimuth; 
}ap3_data_t;


typedef struct
{
	uint32_t start_sense;
	uint32_t end_sense;
	int already_make;
	float delay_in_sec;
	uint32_t sense;
}timeout_delay_t;


#define AP3_ESTIMATION_TIME_SEC    10

#define AP3_FEEDFORWARD_DELAY_SEC

#define AP3_SYNC_RATE              SYSTICK_TIMER_FRQ

#ifdef MAIN

#else
extern uint8_t ap3_parse_done;
extern ap3_estimator_t ap3_estmtr;
extern ap3_data_t ap3_data;
#endif

void ap3_estimator(void);
void ap3_proto_frame_parser(ap3_proto_frame_t* frame);

#endif
