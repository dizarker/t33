#include "crc16_xmodem.h"

//=============================================================================
static uint16_t crc16_xmodem_update(uint16_t crc, uint8_t data)
{
	static const uint16_t CRC_POLY = 0x1021;

	crc ^= (uint16_t)data<<8;

	for (int_fast8_t i=0; i<8; i++)
	{
		crc = (crc & 0x8000) ?  (crc<<1)^CRC_POLY : crc<<1;
	}

	return crc;
}

//=============================================================================
uint16_t crc16_xmodem(uint8_t* buf, size_t len)
{
    uint16_t crc = 0x0000;
 
    while (len--)
	{
		crc = crc16_xmodem_update(crc, *buf++);
	}
 
    return (crc);
}
