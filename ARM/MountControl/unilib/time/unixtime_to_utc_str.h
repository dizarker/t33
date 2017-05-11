#ifndef _UNIXTIME_TO_UTC_STR_H_
#define _UNIXTIME_TO_UTC_STR_H_
#include <time.h>


typedef struct
{
	uint8_t POS;
	uint8_t LENGTH;
}time_service_t;


typedef struct
{
	char str[25];
	volatile uint32_t d32;
	uint32_t length;
	time_service_t YEAR_SERVICE;
	time_service_t MM_DD_HH_MM_SS_SERVICE;
}utc_time_t;


void unixtime_to_UTC_str_init(utc_time_t *utc_time);
void unixtime_to_UTC_str_task(time_t uin32_time, utc_time_t *utc_time);
#endif
