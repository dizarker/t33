/*
 * NMEA protocol implementation (NMEA-0183)
 *
 */

#ifndef _NMEA_H_
#define _NMEA_H_

enum // NMEA FIELDS TYPES
{
	NMF_NONE     = 0,
	NMF_FLOAT    = 1,
	NMF_INT      = 2,
	NMF_CHAR     = 3,
	NMF_HHMMSSMS = 4, // time UTC
	NMF_DDMMYY   = 5, // day,month,year
	NMF_POS      = 6  // (G)GGMM.MM  - latitude or longitude
};

#pragma pack(push, 1)
typedef struct 
{
	union
	{
		double  f64;
		float   f32;
		int32_t i32;
		int64_t i64;
		char    c;
		uint8_t byte[8];

		struct
		{
			uint8_t  dd;
			uint8_t  mm;
			uint8_t  yy;
		} date;

		struct
		{
			uint8_t  hh;
			uint8_t  mm;
			uint8_t  ss;
			uint16_t ms;	
		} time;
	} x;
	int   type;
} nmea_arg_t;
#pragma pack(pop)

int nmea_task(uart_t* const uart);

#endif
