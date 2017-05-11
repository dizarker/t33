#include "include.h"


typedef struct
{
	uint8_t pressure_sensor_error;				//1 bit
	uint8_t motor_dead; 						//2 bit
	uint8_t decompression;						//3 bit
	uint8_t airing;								//4 bit
	uint8_t pressure_good;						//5 bit
	uint8_t wear_gazoline_pump;					//6 bit
	uint8_t critical_wear_gazoline_pump;		//7 bit
	uint8_t case_after_start_first;				//8 bit
	uint8_t case_after_start_second;			//9 bit
	uint8_t case_after_start_third;				//10 bit
	
}benzo_errors_t;


//=============================================================================
static void ecu_benzo_proto_packet_type1_parser(uint8_t* data)
{
	benzo_errors_t benzo_errors;
	
	//dprintf("rx:%d\n", read_word_lendian(&data[0]));
	uint16_t error_from_gazoline_pump = read_word_lendian(&data[0]);
	
	benzo_errors.pressure_sensor_error = error_from_gazoline_pump & 0x01;
	
	benzo_errors.motor_dead						= (error_from_gazoline_pump>>1) & 0x01;
	benzo_errors.decompression					= (error_from_gazoline_pump>>2) & 0x01;
	benzo_errors.airing							= (error_from_gazoline_pump>>3) & 0x01;
	benzo_errors.pressure_good					= (error_from_gazoline_pump>>4) & 0x01;
	benzo_errors.wear_gazoline_pump				= (error_from_gazoline_pump>>5) & 0x01;
	benzo_errors.critical_wear_gazoline_pump 	= (error_from_gazoline_pump>>6) & 0x01;
	benzo_errors.case_after_start_first			= (error_from_gazoline_pump>>7) & 0x01;
	benzo_errors.case_after_start_second		= (error_from_gazoline_pump>>8) & 0x01;
	benzo_errors.case_after_start_third		= (error_from_gazoline_pump>>9) & 0x01;
	
	dprintf("decompr:%d, pressure_good:%d, badGP:%d, veryBadGP:%d\n", benzo_errors.decompression, 
							benzo_errors.pressure_good,
							benzo_errors.wear_gazoline_pump,
							benzo_errors.critical_wear_gazoline_pump);
	
	dprintf("fuel_not_null:%d, fuel_null:%d, signal_bad:%d\n\n\n", benzo_errors.case_after_start_first, 
							benzo_errors.case_after_start_second,
							benzo_errors.case_after_start_third);
	
}

//=============================================================================
void ecu_benzo_proto_frame_parser(ecu_benzo_proto_frame_t* frame)
{
	switch (frame->header.packet_type)
	{
		case PACKET_TYPE1_BENZO_DATA:
		{				
			ecu_benzo_proto_packet_type1_parser(frame->data);
			
			pin_toggle(PIN_LED_GREEN_EXCHANGE);
					
			break;
		}
		
		default:
		{
			break;
		}
	}
}
