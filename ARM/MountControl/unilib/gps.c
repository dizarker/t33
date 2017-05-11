#include "include.h"

static gps_info_t m_gps;

#ifndef GPS_UART
#	error GPS_UART is undefined ! (example: define GPS_UART uart3)
#endif

//=============================================================================
gps_info_t* gps_init(void)
{
	uart_flush(&GPS_UART);

	return (&m_gps);
}

//=============================================================================
int gps_task(void)
{
	return (nmea_task(&GPS_UART));
}

#if defined(GPS_ESTIMATION_TIME_SEC) && (GPS_ESTIMATION_TIME_SEC>0)

static const int GPS_ESTIMATION_TIMEOUT = GPS_SYNC_RATE * GPS_ESTIMATION_TIME_SEC;

//=============================================================================
static uint32_t gps_rtc_to_ms(gps_rtc_t* rtc)
{
	return
	(
		rtc->time.hour * 1000UL * 60UL * 60UL
	  + rtc->time.min  * 1000UL * 60UL
	  + rtc->time.sec  * 1000UL
	  + rtc->time.ms
	);
}

//=============================================================================
static float gps_rtc_dtime_sec(gps_rtc_t* rtc2, gps_rtc_t* rtc1) // (float sec)(t2-t1)   (only HHMMSSDD diff w/o YYYYMMDD)
{
	static const uint32_t MS_PER_DAY = 1000UL*60UL*60UL*24UL;

	uint32_t t2 = gps_rtc_to_ms(rtc2);
	uint32_t t1 = gps_rtc_to_ms(rtc1);
	uint32_t dt = t2 - t1;

	if (t2<t1)  dt += MS_PER_DAY;	  // dt = t2 + (MS_PER_DAY - t1)

	return (dt / 1000.0f); // ms to sec
}

//=============================================================================
void gps_estimator_sync(void) // at GPS_SYNC_RATE
{
	if (m_gps.estimation_timeout<GPS_ESTIMATION_TIMEOUT)
	{
		m_gps.estimation_timeout++;

		double sec = m_gps.estimation_timeout / (double)GPS_SYNC_RATE;
		
		#if defined(GPS_FEEDFORWARD_DELAY_SEC)
			m_gps.pos_proxi.lat = m_gps.pos.lat + m_gps.speed.lat*sec + m_gps.pos_feedforward_offset.lat;
			m_gps.pos_proxi.lon = m_gps.pos.lon + m_gps.speed.lon*sec + m_gps.pos_feedforward_offset.lon;
		#else
			m_gps.pos_proxi.lat = m_gps.pos.lat + m_gps.speed.lat*sec;
			m_gps.pos_proxi.lon = m_gps.pos.lon + m_gps.speed.lon*sec;
		#endif

		
		m_gps.pos_proxi.lat = map_check_if_lat_more_pi_div_two(m_gps.pos_proxi.lat);
		m_gps.pos_proxi.lon = map_check_if_lon_more_pi(m_gps.pos_proxi.lon);
		
		m_gps.updated = 1;
		
		
	}
}

//=============================================================================
static void gps_on_nmea_gprmc_estimator_sync(void)
{
	static gps_rtc_t rtc_prev;
	static double    lat_prev;
	static double    lon_prev;
			
	float sec = gps_rtc_dtime_sec(&m_gps.rtc, &rtc_prev); // (float sec)(t2-t1)    (only HHMMSSDD diff w/o YYYYMMDD)

	if (sec<GPS_ESTIMATION_TIME_SEC)
	{
		m_gps.speed.lat = map_get_diff_btwn_lat(m_gps.pos.lat, lat_prev) / sec; // deg/s
		m_gps.speed.lon = map_get_diff_btwn_lon(m_gps.pos.lon, lon_prev) / sec; // deg/s
		
		#if defined(GPS_FEEDFORWARD_DELAY_SEC)
			m_gps.pos_feedforward_offset.lat = m_gps.speed.lat*GPS_FEEDFORWARD_DELAY_SEC;
			m_gps.pos_feedforward_offset.lon = m_gps.speed.lon*GPS_FEEDFORWARD_DELAY_SEC;			
    	#endif

//		dprintf("vlat:%1.6f, vlon:%1.6f, dt:%3.6f, sec:%d ms:%d\n", m_gps.speed.lat, m_gps.speed.lon, sec, m_gps.rtc.time.sec, m_gps.rtc.time.ms);

		m_gps.estimation_timeout = -1; // will be set to 0 in next line

		gps_estimator_sync();
	}


	rtc_prev = m_gps.rtc;     // backup
	lat_prev = m_gps.pos.lat; 
	lon_prev = m_gps.pos.lon;
}
#endif


/*=============================================================================
 * $GPRMC,hhmmss.ss,A,GGMM.MM,P,gggmm.mm,J,v.v,b.b,ddmmyy,x.x,n,m*hh<CR><LF>
 * 
 * Fields:
 *
 * 0   <hhmmss.ss> - врем€ фиксации местоположени€ по √ринвичу UTC: <hh> - часы, <mm> - минуты, <ss.ss> - секунды. ƒлина дробной части секунд варьируетс€. Ћидирующие нули не опускаютс€.
 * 1   <A> - статус: <A> - данные достоверны, <V> - недостоверны.
 * 2   <GGMM.MM> - широта. 2 цифры градусов(<GG>), 2 цифры целых минут, точка и дробна€ часть минут переменной длины. Ћидирующие нули не опускаютс€.
 * 3   <P> - <N> дл€ северной или <S> дл€ южной широты.
 * 4   <gggmm.mm> - долгота. 3 цифры градусов(<ggg>), 2 цифры целых минут, точка и дробна€ часть минут переменной длины. Ћидирующие нули не опускаютс€.
 * 5   <J> - <E> дл€ восточной или <W> дл€ западной долготы.
 * 6   <v.v> - горизонтальна€ составл€юща€ скорости относительно земли в узлах. „исло с плавающей точкой. ÷ела€ и дробна€ части переменной длины.
 * 7   <b.b> - путевой угол (направление скорости) в градусах. „исло с плавающей точкой. ÷ела€ и дробна€ части переменной длины. «начение равное 0 соответствует движению на север, 90 - восток, 180 - юг, 270 - запад.
 * 8   <ddmmyy> - дата: день мес€ца, мес€ц, последние 2 цифры года (ведущие нули об€зательны).
 * 9   <x.x> - магнитное склонение в градусах (часто отсутствует), рассчитанное по некоторой модели. „исло с плавающей точкой. ÷ела€ и дробна€ части переменной длины.
 * 10  <n> - направление магнитного склонени€: дл€ получени€ магнитного курса магнитное склонение необходимо <E> - вычесть, <W> - прибавить к истинному курсу.
 * 11  <m> - индикатор режима: <A> - автономный, <D> - дифференциальный, <E> - аппроксимаци€, <N> - недостоверные данные (часто отсутствует, данное поле включа€ зап€тую отсутствует в старых верси€х NMEA).
 *=============================================================================
 */
void gps_on_nmea_gprmc(nmea_arg_t* args)
{
	if (args[1].x.c=='A')                // if valid data
	{
		m_gps.rtc.time.hour    = args[0].x.time.hh;
		m_gps.rtc.time.min     = args[0].x.time.mm;
		m_gps.rtc.time.sec     = args[0].x.time.ss;
		m_gps.rtc.time.ms      = args[0].x.time.ms;
		m_gps.rtc.date.day     = args[8].x.date.dd;
		m_gps.rtc.date.month   = args[8].x.date.mm;
		m_gps.rtc.date.year    = args[8].x.date.yy;

		m_gps.pos.lat          = (args[3].x.c=='S') ?  -args[2].x.f64 : args[2].x.f64; // correct latitute for N/S
		m_gps.pos.lon          = (args[5].x.c=='W') ?  -args[4].x.f64 : args[4].x.f64; // correct longitude for E/W

		m_gps.speed.horizontal = args[6].x.f32 * 1.852 * 1000.0 / 3600.0;    // horizontal velocity in knots (1knot = 1.852km/h = 0.51(4)m/s )
		m_gps.azimuth          = args[7].x.f32; // 0-nord 90-east

		#if defined(GPS_ESTIMATION_TIME_SEC) && (GPS_ESTIMATION_TIME_SEC>0)
			gps_on_nmea_gprmc_estimator_sync();
		#else
			m_gps.updated = 1;
		#endif
	}
}

/*=============================================================================
 *        0         1       2 3        4 5 6  7   8   9 10 11 12  13
 *        |         |       | |        | | |  |   |   | |   | |   |
 * $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx
 *
 * Fields:
 *
 *  0  Time (UTC)
 *  1  Latitude
 *  2  N or S (North or South)
 *  3  Longitude
 *  4  E or W (East or West)
 *  5  GPS Quality Indicator,
 *      0 - fix not available,
 *      1 - GPS fix,
 *      2 - Differential GPS fix
 *  6  Number of satellites in view, 00 - 12
 *  7  Horizontal Dilution of precision
 *  8  Antenna Altitude above/below mean-sea-level (geoid) 
 *  9  Units of antenna altitude, meters
 * 10  Geoidal separation, the difference between the WGS-84 earth
 *     ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
 * 11  Units of geoidal separation, meters
 * 12  Age of differential GPS data, time in seconds since last SC104
 *     type 1 or 9 update, null field when DGPS is not used
 * 13  Differential reference station ID, 0000-1023
 *=============================================================================
 */
void gps_on_nmea_gpgga(nmea_arg_t* args)
{
	m_gps.satelites_qty = args[6].x.i32;
	m_gps.pos.alt       = args[8].x.f32;
	
}

//=============================================================================
void gps_on_nmea_gpgsa(nmea_arg_t* args)
{
	
}
