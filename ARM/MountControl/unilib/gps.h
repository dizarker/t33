#ifndef _GPS_H_
#define _GPS_H_

#include "gps_conf.h"

typedef struct
{
	float horizontal;   // horizontal velocity in m/s
	float lat;          
	float lon;	        
} speed_t;

typedef struct
{
	double lat;         // 0-2Pi	0-360
	double lon;	        // 0-2Pi	0-360
	float  alt;	        // meters
	float  alt_offset;
} pos_lla_t;

typedef struct
{
	struct
	{
		uint_fast8_t  year;
		uint_fast8_t  month;
		uint_fast8_t  day;
	} date;

	struct
	{
		uint_fast8_t  hour;
		uint_fast8_t  min;
		uint_fast8_t  sec;
		uint_fast16_t ms;
	} time;
} gps_rtc_t;

typedef struct
{	
	int       updated;
	int       satelites_qty;
	float     azimuth;
	speed_t   speed;
	gps_rtc_t rtc;
	pos_lla_t pos;                    // original pose w/o estimation

	#if defined(GPS_ESTIMATION_TIME_SEC) && (GPS_ESTIMATION_TIME_SEC>0)
		pos_lla_t pos_proxi;              // can be estimated (aproximated), next fields are used for this
		#if defined(GPS_FEEDFORWARD_DELAY_SEC)
			pos_lla_t pos_feedforward_offset; // feedforward offset - is for delay compensation 
		#endif
		int       estimation_timeout;
	#endif

} gps_info_t;
			
//=============================================================================
gps_info_t* gps_init          (void);
int         gps_task          (void);
void        gps_estimator_sync(void);  // at GPS_SYNC_RATE

void        gps_on_nmea_gprmc (nmea_arg_t* args);
void        gps_on_nmea_gpgga (nmea_arg_t* args);
void        gps_on_nmea_gpgsa (nmea_arg_t* args);

#endif
