#include "include.h"

/*
*	Set the default settings for pos YEAR and MM_DD_HH_MM_SS
*	This settings are standart for str, whome returned by function ctime();
*/
void unixtime_to_UTC_str_init(utc_time_t *utc_time)
{
	utc_time->YEAR_SERVICE.POS 		= 20;
	utc_time->YEAR_SERVICE.LENGTH 	= 4;
	
	utc_time->MM_DD_HH_MM_SS_SERVICE.LENGTH = 16;
	utc_time->MM_DD_HH_MM_SS_SERVICE.POS	= 3;
}

//=============================================================================
void unixtime_to_UTC_str_task(time_t uin32_time, utc_time_t *utc_time)
{
	//char *p = &utc_time->str[0];
	
	//char *p = ctime(&uin32_time);
	
	
	memcpy(&utc_time->str[0], ctime(&uin32_time), 25);
}

