#include "include.h"
 
/*
* 	Last ver. 2015.12.25.
*		Rename from map_local to map_light and del function,  which works with line. 
*		Deleted function you can watch in map_local project AFS.
* 	Last ver. 2015.06.29.
* 		add functions:
* 			map_get_diff_btwn_lat(), map_get_diff_btwn_lon(), 
*			map_check_if_lon_more_pi(), map_check_if_lat_more_pi_div_two()
*		its make for work in different hemisphere of Earth.
*
*
*
*/

//=============================================================================
double map_check_if_lat_more_pi_div_two(double lat)
{
	if(lat > 90)
	{
		lat = 180.0 - lat;	
	}
	if(lat < -90)
	{	
		lat = lat + 180.0;
	}
	return lat;
}

//=============================================================================
double map_check_if_lon_more_pi(double lon)
{
	if(lon > 180)
	{
		lon = 360.0 - lon;	
	}
	if(lon < -180)
	{		
		lon = lon + 360.0;
	}
	return lon;
}

//=============================================================================
double map_get_diff_btwn_lat(double lat_new, double lat_old)
{
	double diff = lat_new - lat_old;
	
	if(ABS(diff) > 90) //set max diff in grad 
	{
		if(lat_old < 0)
		{			
			diff =  diff - 180.0;
		}
		if(lat_new < 0)
		{			
			diff = 180.0 + diff;
		}
	}
	
	return diff;
}

//=============================================================================
double map_get_diff_btwn_lon(double lon_new, double lon_old)
{
	double diff = lon_new - lon_old;
		
	if(ABS(diff) > 180.0) //set max diff in grad
	{
		if(lon_old < 0)
		{			
			diff =  diff - 360.0;
		}
		if(lon_new < 0)
		{			
			diff = 360.0 + diff;
		}
	}	
	
	return diff;
}

//=============================================================================
float map_distance(float lat1, float lon1, float lat2, float lon2, float klon)
{
	// lat[-90..+90]   -> dlat[-180..+180] - ok                                      
	// lon[-180..+180] -> dlon[-360..+360] - bad (not short arc on shera is possible)

	static const float r       = 6372795.0;          // earth radius
	static const float deg2rad = PI / 180.0;


	lon1 *= deg2rad;
	lat1 *= deg2rad;

	lon2 *= deg2rad;
	lat2 *= deg2rad;

	float dlat = lat2 - lat1;	
	
	float dlon = fabsf(lon2 - lon1);
	
	if (dlon>PI)
	{
		dlon -= 2.0*PI; // make short arc on speroid
	}
	
	dlon = dlon * klon; // longitude compression factor
	
	return (r * hypotf(dlon, dlat));
}

//=============================================================================
float map_get_medium_btwn_two_lon(float lon1, float lon2)
{		
	float medium = (lon1 + lon2)/2;
	
	if( (lon1 < 0 && lon2 > 0) || (lon1 > 0 && lon2 < 0) )
	{			
		float delta = map_get_diff_btwn_lon(lon1, lon2) / 2;
							
		if(ABS(lon1) >ABS(lon2))
		{
			medium = lon2 + delta;						
		}
		else
		{
			medium = lon1 - delta;					
		}						
	}

	return (medium);
}

//=============================================================================
static float triangle_hc(float a, float b, float c)
{
	if (fabsf(c) <= FLT_MIN)
	{
		return ((a+b)/2.0f);
	}

	float p = (a+b+c)/2.0;  // half of perimeter

	float s = sqrtf(p*(p-a)*(p-b)*(p-c)); // Heron's formula

	return (s*2.0/c);  // hc
}

//=======================================================================================
void map_get_pos_wo_feedforward_offset(pos_ll_t *dst, pos_lla_t* curr, pos_lla_t* offset)
{	
	dst->lat = map_get_diff_btwn_lat(curr->lat, offset->lat);
	dst->lon = map_get_diff_btwn_lon(curr->lon, offset->lon);
}
