/* 
 * Main map of points module.
 *
 *
 */

#ifndef _MAP_LOCAL_H_
#define _MAP_LOCAL_H_

typedef struct
{
	float lat;
	float lon;
}pos_ll_t;


float       map_distance      					(float lat1, float lon1, float lat2, float lon2, float klon);
float 		map_get_medium_btwn_two_lon			(float lon1, float lon2);

double 		map_get_diff_btwn_lat				(double lat_new, double lat_old);
double 		map_get_diff_btwn_lon				(double lon_new, double lon_old);
double 		map_check_if_lon_more_pi			(double lon);
double 		map_check_if_lat_more_pi_div_two	(double lat);
void 		map_get_pos_wo_feedforward_offset	(pos_ll_t *dst, pos_lla_t* curr, pos_lla_t* offset);

#endif
