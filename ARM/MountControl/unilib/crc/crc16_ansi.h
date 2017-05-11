/*
 *  crc16.h rev.2.2.0 2010-06-07
 *
 *  CRC-16-ANSI (IBM) algorithm:
 *
 *  Polynomial: reversed x^16 + x^15 + x^2 + 1 (0xa001)
 *  Initial value: 0xffff
 *
 *  This CRC is normally used in disk-drive controllers,
 *  and protocols: MODBUS, USB, Bisync, X3.28.
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 */

#ifndef _CRC16_ANSI_H_
#define _CRC16_ANSI_H_

#include <stdlib.h>
#include <inttypes.h>

uint16_t crc16_ansi(uint8_t* buf, size_t len);

#endif 
