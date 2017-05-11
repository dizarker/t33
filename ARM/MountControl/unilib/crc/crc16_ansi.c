#include "crc16_ansi.h"

//=============================================================================
static uint16_t crc16_update(uint16_t crc, uint8_t x)
{
	static const uint16_t CRC_POLY = 0xA001;

	crc ^= x;

	for (int_fast8_t i=0; i<8; ++i)
	{
	    crc = (crc & 1U) ?  (crc>>1)^CRC_POLY : crc>>1;
	}

	return (crc);
}

//=============================================================================
uint16_t crc16_ansi(uint8_t *buf, size_t len)
{
    uint16_t crc = 0xffff;

    while (len--)
    {
        crc = crc16_update(crc, *buf++);
    }

    return (crc);
}
