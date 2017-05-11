#ifndef _COOR_MATH_H_
#define _COOR_MATH_H_


typedef struct
{
	float lat;
	float lon;
	float alt;
}pos_ll_t;

pos_ll_t 	get_coordinate	(pos_ll_t* home, float distance, float azimuth);
float 		get_azimuth		(pos_ll_t* dest, pos_ll_t* home);
float 		get_distance	(pos_ll_t* dest, pos_ll_t* home);
uint32_t 	coor_get_hemisphere		(float lat, float lon);
uint32_t coor_check_if_exit_from_region		(pos_ll_t *pos);
#endif
