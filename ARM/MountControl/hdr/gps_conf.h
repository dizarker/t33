#ifndef _GPS_CONF_H_
#define _GPS_CONF_H_

#define GPS_UART                   uart_gps

// uncomment next two lines to enable gps estimator:

#define GPS_ESTIMATION_TIME_SEC    10

#define GPS_FEEDFORWARD_DELAY_SEC  0.1f

#define GPS_SYNC_RATE              SYS_TIMER_FRQ

#endif
