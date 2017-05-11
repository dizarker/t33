#include "include.h"

/*
*	last ver. 2015.06.29
*		1. Lat, lon must be signed.
*		2. ap3_estmtr.speed.lat, ap3_estmtr.speed.lon compt with help of function map_get_diff_btwn_lat()..
*		3. Add check lat, lon - map_check_if_lat_more_pi_div_two(), map_check_if_lon_more_pi();
*
*
*
*/


uint8_t ap3_parse_done = 0;
ap3_data_t ap3_data;
ap3_estimator_t ap3_estmtr;

static pos_t pos;
static timeout_delay_t timeout_btwn_coor;
static uint32_t check_new_coor(pos_t* pos);
static void ap3_coor_estimator_sync(float sec);



//=============================================================================
static void ap3_proto_packet_type5_parser(uint8_t* data)
{
	#define UTIME_OFFSET  			(5-5)
	#define ROLL_OFFSET   			(19-5)
	#define PITCH_OFFSET  			(21-5)
	#define YAW_OFFSET    			(23-5)	
	#define AIR_SPEED_OFFSET    	(25-5)	
	#define AIR_ALT_OFFSET	    	(26-5)
	#define CURR_POINT_OFFSET    	(53-5)
	#define GPS_AZIMUTH_OFFSET 		(51-5)
	#define MAG_OFFSET    			(59-5)
	#define LON_OFFSET    			(41-5)
	#define LAT_OFFSET		    	(45-5)	
	#define ALT_OFFSET    			(49-5)
	#define EVENTS_OFFSET    		(79-5)
	
	#define GPS_SPEED_OFFSET    	(81-5)
	
	
/*		Example pars
	photo.status.info.ypr[0]      = 0.1f*(int16_t)read_word_bendian(&data[YAW_OFFSET]);	
	photo.status.info.ypr[1]      = 0.1f*(int16_t)read_word_bendian(&data[PITCH_OFFSET]);
	photo.status.info.ypr[2]      = 0.1f*(int16_t)read_word_bendian(&data[ROLL_OFFSET]);
	photo.status.info.utime       = read_dword_bendian(&data[UTIME_OFFSET]);
	photo.status.info.mag_azimuth = 2.0f*(uint8_t)data[MAG_OFFSET];
	track.curr_point			  = (uint8_t)data[CURR_POINT_OFFSET];
	
	pos.lat = (double)((int32_t)read_dword_bendian(&data[LAT_OFFSET]))/1000000; //grad
	pos.lon = (double)((int32_t)read_dword_bendian(&data[LON_OFFSET]))/1000000; //grad
	
	photo.status.info.alt = ap3_estmtr.alt;
	
	ap3_estmtr.alt 		= (float)read_word_bendian(&data[ALT_OFFSET]);
	ap3_data.event		= (uint8_t)data[EVENTS_OFFSET];	
	ap3_data.air_speed 	= (uint8_t)data[AIR_SPEED_OFFSET];	
	ap3_data.air_alt 	= 0.2f*(int16_t)read_word_bendian(&data[AIR_ALT_OFFSET]);
	ap3_data.gps_azimuth= 0.1f*(uint16_t)read_word_bendian(&data[GPS_AZIMUTH_OFFSET]);	
	ap3_data.gps_speed	= (float)data[GPS_SPEED_OFFSET];
*/
	timeout_btwn_coor.end_sense = systick_timer.tics;
	
	if(check_new_coor(&pos))
	{		
		timeout_btwn_coor.delay_in_sec 		= (float)(timeout_btwn_coor.end_sense - timeout_btwn_coor.start_sense)/200;//attention change this shit if another sense of systics_timer
		timeout_btwn_coor.start_sense 	= timeout_btwn_coor.end_sense;
		ap3_coor_estimator_sync(timeout_btwn_coor.delay_in_sec);
		pin_toggle(PIN_LED_RED);  // show ap3 activity
	}

}

//=============================================================================
void ap3_proto_frame_parser(ap3_proto_frame_t* frame)
{
	uint16_t talker_id = read_word_bendian((uint8_t*)&frame->header.talker_id_bendian);

	if (talker_id == TALKER_ID_AUTOPILOT)
	{
		switch (frame->header.packet_type)
		{
			case PACKET_TYPE5_LOGGER_DATA:
			{
				ap3_proto_packet_type5_parser(frame->data);
				break;
			}

			default:
			{
				break;
			}
		}
	}
}


#if defined(AP3_ESTIMATION_TIME_SEC) && (AP3_ESTIMATION_TIME_SEC>0)

static const int AP3_ESTIMATION_TIMEOUT = AP3_SYNC_RATE * AP3_ESTIMATION_TIME_SEC;
//=============================================================================
static uint32_t check_new_coor(pos_t* pos)
{	
	if(pos->lat != ap3_estmtr.pos_real.lat && pos->lon != ap3_estmtr.pos_real.lon)
	{				
		ap3_estmtr.pos_real.lon = pos->lon;
		ap3_estmtr.pos_real.lat = pos->lat;
		return 1;
	}
	return 0;
}


//=============================================================================
void ap3_estimator(void) // at GPS_SYNC_RATE
{
	if(ap3_estmtr.timeout <AP3_ESTIMATION_TIMEOUT)
	{
		ap3_estmtr.timeout++;

		float sec = ap3_estmtr.timeout / (float)AP3_SYNC_RATE;

		#if defined(AP3_FEEDFORWARD_DELAY_SEC)
			ap3_estmtr.pos_proxi.lat = ap3_estmtr.pos_real.lat + ap3_estmtr.speed.lat*sec + ap3_estmtr.pos_feedforward_offset.lat;
			ap3_estmtr.pos_proxi.lon = ap3_estmtr.pos_real.lon + ap3_estmtr.speed.lon*sec + ap3_estmtr.pos_feedforward_offset.lon;			
		#else
			ap3_estmtr.pos_proxi.lat = ap3_estmtr.pos_real.lat + ap3_estmtr.speed.lat*sec;
			ap3_estmtr.pos_proxi.lon = ap3_estmtr.pos_real.lon + ap3_estmtr.speed.lon*sec;
		#endif
				
		//dprintf("ln=%f\n", ap3_estmtr.pos_proxi.lon);
		
		ap3_estmtr.pos_proxi.lat = map_check_if_lat_more_pi_div_two(ap3_estmtr.pos_proxi.lat);
		ap3_estmtr.pos_proxi.lon = map_check_if_lon_more_pi(ap3_estmtr.pos_proxi.lon);
		
		//dprintf("ln1=%f\n", ap3_estmtr.pos_proxi.lon);
			
		ap3_estmtr.update = 1;
		
	}
	else{ ap3_estmtr.update = 0; }
}

//=============================================================================
// calculate: 
//            vlat,
//            vlon, 
//            feedforward_offset.lat
//            feedforward_offset.lat
/*
*	if first: lat_prev = 0 and lon_prev = 0
*		then speed.lat = big bad digital and speed.lon = big bad digital
*	then whole circle for next update proxi coordinates will be bad.
*
*/
static void ap3_coor_estimator_sync(float sec)
{
	static double    lat_prev;
	static double    lon_prev;
		
	if (sec<AP3_ESTIMATION_TIMEOUT)
	{			
		ap3_estmtr.speed.lat = map_get_diff_btwn_lat(ap3_estmtr.pos_real.lat, lat_prev) / sec; // deg/s							// deg/s
		ap3_estmtr.speed.lon = map_get_diff_btwn_lon(ap3_estmtr.pos_real.lon, lon_prev) / sec; // deg/s

		#if defined(AP3_FEEDFORWARD_DELAY_SEC)
			ap3_estmtr.pos_feedforward_offset.lat = ap3_estmtr.speed.lat * ap3_estmtr.feedforward_delay_sec;
			ap3_estmtr.pos_feedforward_offset.lon = ap3_estmtr.speed.lon * ap3_estmtr.feedforward_delay_sec;
    	#endif

		ap3_estmtr.timeout = -1;

		lat_prev = ap3_estmtr.pos_real.lat;
		lon_prev = ap3_estmtr.pos_real.lon;

	}
	else
	{
//		dprintf("BAD AP3_ESTIMATION_TIME_SEC: %6.3f\n", sec);
	}
}


#endif
