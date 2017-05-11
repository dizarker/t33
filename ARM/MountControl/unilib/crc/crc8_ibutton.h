/*
 *  crc8.h rev.2.2.0 2010-06-07
 *
 *  CRC-8 iButton Dallas (Maxim) algorithm:
 *
 *
 *  Polynomial revert: x^8 + x^5 + x^4 + 1 (0x8C)
 *  Initial value: 0x00
 *
 *  See http://www.maxim-ic.com/appnotes.cfm/appnote_number/27
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 */



#ifndef _CRC8_IBUTTON_H_
#define _CRC8_IBUTTON_H_

#include <stdlib.h>
#include <inttypes.h>

uint8_t crc8_ibutton(uint8_t* buf, size_t len);

#endif 
