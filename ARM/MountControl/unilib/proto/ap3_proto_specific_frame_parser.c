#include "include.h"


#define AP3_KEY_OFFSET		(12-5)	
#define AP3_SCRAMBLE_DATA	(41-5)
	
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


//=============================================================================
static void ap3_proto_packet_type5_parser(uint8_t* data)
{
	if(!utc_time.d32)
	{
		utc_time.d32 	= read_dword_bendian(&data[UTIME_OFFSET]);
		unixtime_to_UTC_str_task(utc_time.d32, &utc_time);
	}
}

//=============================================================================
void ap3_proto_frame_parser(ap3_proto_frame_t* frame)
{
	uint16_t talker_id = read_word_bendian((uint8_t*)&frame->header.talker_id_bendian);

	if (talker_id == TALKER_ID_MAGNITOMETR)
	{
		switch (frame->header.packet_type)
		{
			case PACKET_TYPE5_LOGGER_DATA:
			{
				ap3_proto_packet_type5_parser(frame->data);
													
				write_in_file.qty_bytes = frame->header.all_size + AP3_PROTO_PREFIX_SIZE + AP3_PROTO_CRC_SIZE + LOGGER_WITH_MAGNITOMETR_ADDITIONAL_SIZE;
				
				pin_toggle(PIN_BLINK_GREEN_LED);
								
				ap3_proto_handle.update = 1;

				break;
			}

			default:
			{
				break;
			}
		}
	}
}
