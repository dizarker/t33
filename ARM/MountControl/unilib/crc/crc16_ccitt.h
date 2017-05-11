/*
 *  crc16.h rev.2.2.0 2010-06-07
 *
 *  CRC-16 CCITT
 *
 *  Poly  : 0x1021 (0x8408)    x^16 + x^12 + x^5 + 1
 *  Initial value: 0xffff
 *
 *  This CRC is normally used by PPP and IrDA (See RFC1171 (PPP protocol) and IrDA IrLAP 1.1)
 *
 *  note: Although the CCITT polynomial is the same as that used by the Xmodem
 *  protocol, they are quite different. The difference is in how the bits are
 *  shifted through the alorgithm. Xmodem shifts the MSB of the CRC and the
 *  input first, while CCITT shifts the LSB of the CRC and the input first.
 *
 *   The following is the equivalent functionality written in C.
 *
 *   uint16_t crc_ccitt_update(uint16_t crc, uint8_t data)
 *   {
 *       data ^= lo8 (crc);
 *       data ^= data << 4;
 *
 *       return ((((uint16_t)data << 8) | hi8 (crc)) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
 *   }
 *
 *  Check : 0x29B1 ("123456789")
 *  MaxLen: 4095 bytes (32767 bits) 
 *  Find next errors: single, double, tripple and all odd
 *
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 */


#ifndef _CRC16_CCITT_H_
#define _CRC16_CCITT_H_

#include <stdlib.h>
#include <inttypes.h>

uint16_t crc16_ccitt        (const void* buf, size_t len);
uint16_t crc16_ccitt_append (uint_fast16_t crc, const void* buf, size_t len);

#endif 
