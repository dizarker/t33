#include "include.h"
static const float MetrinSec = 30.83;
static const float TOGRAD = 3600;
static const float R = 6371;
//-------------------------------------------------------------------------------------
pos_lla_t get_coordinate(pos_lla_t* home, float distance, float azimuth)
{
    pos_lla_t pos_dest;
    distance = distance/1000;   //to km if old compt
    azimuth = DEG2RAD(azimuth);

    float lat1 = DEG2RAD(home->lat);
    float lon1 = DEG2RAD(home->lon);

    pos_dest.lat = asin(sin(lat1) * cos(distance/R) + cos(lat1) * sin(distance/R) * cos(azimuth));

    pos_dest.lon = lon1 + atan2( sin(azimuth) * sin(distance/R) * cos(lat1), cos(distance/R) - sin(lat1) * sin(pos_dest.lat) );

    pos_dest.lat = RAD2DEG(pos_dest.lat);
    pos_dest.lon = RAD2DEG(pos_dest.lon);
	
	if(pos_dest.lon > 180)
	{
		pos_dest.lon = pos_dest.lon - 360;
	}
	else if(pos_dest.lon < -180)
	{
		pos_dest.lon = 360 + pos_dest.lon;
	}

    return pos_dest;				
};


//-------------------------------------------------------------------------------------
float get_distance(pos_lla_t* dest, pos_lla_t* home)
{
	float dlat = dest->lat - home->lat;
	float dlon = dest->lon - home->lon;
	float m2secLat = MetrinSec * cos(DEG2RAD(home->lat));

	float grad2m_dlat = (dlat * TOGRAD) * MetrinSec;
	float grad2m_dlon = (dlon * TOGRAD) * m2secLat;
	
	float d = sqrt( (grad2m_dlat * grad2m_dlat) + (grad2m_dlon * grad2m_dlon));
    return (d/1000);	 //kmetres
};

//-------------------------------------------------------------------------------------
float get_azimuth(pos_lla_t* dest, pos_lla_t* home)
{
    //static const float R = 6371;
	float lat_dest;
	float lon_dest;
	float lat_home;
	float lon_home;

  	lat_dest = DEG2RAD(dest->lat);	 
	lon_dest = DEG2RAD(dest->lon);

    lat_home = DEG2RAD(home->lat);    
    lon_home = DEG2RAD(home->lon);

    float dlon = lon_home - lon_dest;
	
    float azimuth1 = atan2(sin(dlon) * cos(lat_home),
                            cos(lat_dest) * sin(lat_home) - sin(lat_dest) * cos(lat_home) * cos(dlon));
    azimuth1 = RAD2DEG(azimuth1);
    
    if(azimuth1 < 0){azimuth1 = (unsigned int)(azimuth1 + 360)%360;}

    return azimuth1;
};

//-------------------------------------------------------------------------------------
uint32_t coor_get_hemisphere(float lat, float lon)
{
	//NE
	if(lat > 0 && lon > 0)
	{
		return 1;
	}
	//NW
	if(lat > 0 && lon < 0)
	{
		return 2;
	}
	//SW
	if(lat < 0 && lon < 0)
	{
		return 3;
	}
	//SE
	if(lat < 0 && lon > 0)
	{
		return 4;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
/*	
* First case:
* start lat: 59.999 old lat: 60.063
* int32_t = 59 - 60
* res: ABS( -1) >= 1
* we change the curr region with lat 59
* why int32_t? is for correct diff: (int32_t)(59.9) - (int32_t)(60.01)
*
* Second case:
* start lat: 179.999 old lat: -179.999
* int32_t = 179 - (-179)
* res: ABS( -1) >= 1
* we change the curr region with
* why int32_t? is for correct diff: (int32_t)(59.9) - (int32_t)(60.01)
*
* Third case:
* start lat: -0.001 old lat: 0.001
* int32_t = (-0) - (0)
* res: ABS( 0) >= 1
* we change the curr region with lat 59
* why int32_t? is for correct diff: (int32_t)(59.9) - (int32_t)(60.01)
* not correct
* and other check is signbit()
*/
uint32_t coor_check_if_exit_from_region(pos_ll_t *pos)
{
	static pos_ll_t pos_old;
	
	if( (signbit(pos_old.lat) !=  signbit(pos->lat)) || (signbit(pos_old.lon) != signbit(pos->lon)) )
	{
		pos_old = *pos;
		return 1;
	}
	if( ABS((int32_t)pos_old.lat - (int32_t)pos->lat)>=1 || ABS((int32_t)pos_old.lon - (int32_t)pos->lon)>=1)
	{
		pos_old = *pos;
		return 1;
	}
	
	return 0;
}