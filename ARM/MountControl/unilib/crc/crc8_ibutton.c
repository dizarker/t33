#include "crc8_ibutton.h"

//=============================================================================
static uint_fast8_t crc8_ibutton_update(uint_fast8_t crc, uint_fast8_t data)
{
	static const uint_fast8_t CRC_POLY = 0x8c;

	crc ^= data;

	for (int_fast8_t i=0; i<8; i++)
	{
	    crc = (crc & 1U) ?  (crc>>1)^CRC_POLY : crc>>1;
	}

	return (crc);
}

//=============================================================================
uint8_t crc8_ibutton(uint8_t* buf, size_t len)
{
	uint_fast8_t crc = 0xff;
 
	while (len--)
	{
		crc = crc8_ibutton_update(crc, *buf++);
	}
 
	return (crc);
}
