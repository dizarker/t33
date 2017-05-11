#include "mc_wind_estimator.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <stdio.h>

typedef struct mc_wind_plane_t_
{
	float x;
	float y;
} mc_wind_plane_t;

#define MC_POINTS 				600
#define MAX_WIND				100
#define RATE					0.5
#define ITERATIONS				7
#define RAND_POINTS				200

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

float distation_square(const mc_wind_plane_t A, const mc_wind_plane_t B)
{
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

float distation(const mc_wind_plane_t A, const mc_wind_plane_t B)
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}

uint8_t mc_wind_estimate(const mc_wind_ap3_data_t buf[], size_t buf_len, uint32_t *ticks, wind_result_t *result)
{
	static mc_wind_plane_t buf1[MC_POINTS], buf2[MC_POINTS]; 
	static mc_wind_plane_t *cbuf = buf1, *nbuf = buf2;
	static mc_wind_plane_t zero;
	static size_t cbuf_size = 0;
	static size_t cbuf_idx = 0;
	static size_t nbuf_size = 0;
	static uint8_t initialized = 0;
	static uint8_t wind_error = WIND_ERROR_OK;

	static double avg_radius;
	static double rect_width;
	static double sum_dist;
	static double best_sum_dist;
	static mc_wind_plane_t best_pnt;
	static mc_wind_plane_t pnt;
	static mc_wind_plane_t prev;
	static uint16_t iter;
	static uint16_t rand_idx;
	static uint16_t radius_idx;
	static uint16_t data_idx;

	if (!initialized)
	{
		srand((size_t) ticks); // TODO initialize with smth more appropriate
		srand(time(0)); // TODO initialize with smth more appropriate
		zero.x = 0;
		zero.y = 0;
		initialized = 1;
	}			

	//fill nbuf
	if (buf_len)
	{
		size_t i;
		for (i = 0; i < buf_len; ++i)
		{
			size_t j;
			if (nbuf_size < MC_POINTS)
			{
				nbuf[nbuf_size].x = buf[i].speed * sin(buf[i].course / 180.0 * M_PI);
				nbuf[nbuf_size].y = buf[i].speed * cos(buf[i].course / 180.0 * M_PI);
				nbuf_size ++;
			}
			if (nbuf_size >= MC_POINTS)
			{
				if (i < buf_len - 1)
					printf("Warning! Some data will be ignored\n");
				break;
			}
		}
		if (nbuf_size == MC_POINTS)
		{
			mc_wind_plane_t *x = cbuf;
			cbuf = nbuf;
			nbuf = x;
			cbuf_size = nbuf_size;
			nbuf_size = 0;
			cbuf_idx = 0; // TODO Think about accumulation mechanism

			iter = 0;
			rand_idx = 0;
			prev.x = 0;
			prev.y = 0;
			rect_width = MAX_WIND;

			if (wind_error == WIND_ERROR_NOT_READY)
			{
				//We haven't estimate wind before we get new buffer

				return WIND_ERROR_ELAPSED_TIME;
			}

			wind_error = WIND_ERROR_NOT_READY;
		}	
	}

	if (cbuf_size != MC_POINTS)
		return WIND_ERROR_NOT_READY;
	if (wind_error == WIND_ERROR_OK || wind_error == WIND_ERROR_INACCURATE)
		return WIND_ERROR_NOT_READY;

	
	for (; *ticks && iter < ITERATIONS; ++iter)
	{
		for (; *ticks && rand_idx < RAND_POINTS; ++rand_idx)
		{
			static mc_wind_plane_t pnt;
			if (data_idx >= cbuf_size)
			{
				pnt.x = rand() % ((int)rect_width * 100) / 100.0 + prev.x - rect_width / 2;
				pnt.y = rand() % ((int)rect_width * 100) / 100.0 + prev.y - rect_width / 2;
//				printf("%.2lf %.2lf\n", pnt.x, pnt.y);
				data_idx = 0;
				radius_idx = 0;				
				avg_radius = 0;
				sum_dist = 0;
			}
			for (; *ticks && radius_idx < cbuf_size; ++radius_idx)
			{
				avg_radius += distation(pnt, cbuf[radius_idx]);
				if (radius_idx == cbuf_size - 1)
				{
					avg_radius /= cbuf_size;
//					printf("AVG RAD: %.2lf", avg_radius);
				}
			}
			for (; *ticks && data_idx < cbuf_size; ++data_idx)
			{
				double d = avg_radius - distation(pnt, cbuf[data_idx]);
				sum_dist += d*d;
			}
			if (data_idx >= cbuf_size)
			{
				if (sum_dist < best_sum_dist)
				{
					best_sum_dist = sum_dist;
					best_pnt.x = pnt.x; best_pnt.y = pnt.y;
				}
			}

		}	
		if (rand_idx >= RAND_POINTS)
		{
			prev.x = best_pnt.x; prev.y = best_pnt.y;
			best_sum_dist = 10000000000;
			rect_width *= RATE;
			rand_idx = 0;
			data_idx = cbuf_size;
//			printf("%.2lf %.2lf\n", best_pnt.x, best_pnt.y);
//			printf("---------------\n");
		}
	}
	
	if (!*ticks)
		return WIND_ERROR_NOT_READY;


	result->speed = distation(zero, best_pnt);
	result->course = acos(best_pnt.y / result->speed);
	result->course *= (best_pnt.x < 0 ? -1 : 1) * 180 / M_PI;

	return wind_error = WIND_ERROR_OK;
}
