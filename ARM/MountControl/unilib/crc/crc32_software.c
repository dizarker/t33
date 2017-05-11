#include "include.h"


/*
*	2015.05.26 - v.1.
*	The source code take from: 
*	http://we.easyelectronics.ru/STM32/crc32-na-stm32-kak-na-kompe-ili-na-kompe-kak-na-stm32.html
*	
*
*
*
*
*/

static uint32_t crc32_table[256];
static uint32_t crc32r_table[256];


#define CRC32_POLY   0x04C11DB7
#define CRC32_POLY_R 0xEDB88320

//=============================================================================
void crc32sftwr_init(void)
{
	int i, j;
	uint32_t c, cr;
	for (i = 0; i < 256; ++i)
	{
		cr = i;
		c = i << 24;
		for (j = 8; j > 0; --j)
		{
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
			cr = cr & 0x00000001 ? (cr >> 1) ^ CRC32_POLY_R : (cr >> 1);
		}
		crc32_table[i] = c;
		crc32r_table[i] = cr;
		//dprintf("crc32r_table[%u] = %X ", i, crc32r_table[i]);
	}
}

//=============================================================================
uint32_t crc32_sftwr(uint32_t init_crc, uint8_t *buf, int len)
{
	uint32_t v;
	uint32_t crc;
	crc = ~init_crc;
	while(len > 0)
	{
		v = *buf++;
		crc = ( crc >> 8 ) ^ crc32r_table[( crc ^ (v ) ) & 0xff];
		len--;
	}
	return ~crc;
}

