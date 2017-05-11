/* Wind estimator, based on GPS data and Monte-Carlo algorithm
 * Author: Dmitry Kovalev */

#ifndef _MC_WIND_ESTIMATOR_H_
#define _MC_WIND_ESTIMATOR_H_

#include "wind_common.h"

typedef struct mc_wind_ap3_data_t_
{
	uint16_t 	course; // deg
	uint8_t 	speed;  // km/h
} mc_wind_ap3_data_t;

uint8_t mc_wind_estimate(const mc_wind_ap3_data_t buf[], size_t buf_len, uint32_t *ticks, wind_result_t *result);

#endif
