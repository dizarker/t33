#include <inttypes.h>
#include <stdlib.h>
#ifndef _WIND_COMMON_H_
#define _WIND_COMMON_H_

#define WIND_ERROR_OK 		0
#define WIND_ERROR_NOT_READY	1
#define WIND_ERROR_INACCURATE 	2
#define WIND_ERROR_ELAPSED_TIME 3

typedef struct wind_result_t_
{
	float		speed; 	// km/h
	float		course; // deg
	uint8_t		error_code; 
} wind_result_t;

#endif
