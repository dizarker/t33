#include "include.h"

/*
*	2016.03.30 - v.2 
*		Check crc:	http://www.lammertbies.nl/comm/info/crc-calculation.html
*	
*
*/
//static uint32_t crc32hw_byte(uint8_t byte, int clear);

//=============================================================================
void crc32hw_init(void)
{
	#if F1_CHECK
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE); 
	#else
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	#endif
}

//=============================================================================
static  uint32_t reverse_32(uint32_t data)//inline
{	
	return __RBIT(data);
};

//=============================================================================
uint32_t crc32hw(const void *buf, int len, int clear)
{
	const uint32_t *p = (uint32_t*) buf;
	
	uint32_t crc;	
	uint32_t crc_reg = 0xffffffff;
	if(clear) CRC_ResetDR();
		
	while(len >= 4)
	{		
		uint32_t help = *p++;	//save sense, otherwise hard fault		
		crc_reg = CRC_CalcCRC(reverse_32(help));
		len -= 4;
	}
	
	crc = reverse_32(crc_reg);

	if(len)
	{
		uint32_t test = CRC_CalcCRC(crc_reg);	//reset to 0	
		
		switch(len)
		{
			case 1:
				crc_reg = CRC_CalcCRC(reverse_32((*p & 0xFF) ^ crc) >> 24);
				crc = ( crc >> 8 ) ^ reverse_32(crc_reg);
				break;
			case 2:
				crc_reg = CRC_CalcCRC(reverse_32((*p & 0xFFFF) ^ crc) >> 16);
				crc = ( crc >> 16 ) ^ reverse_32(crc_reg);
				break;
			case 3:
				crc_reg = CRC_CalcCRC(reverse_32((*p & 0xFFFFFF) ^ crc) >> 8);
				crc = ( crc >> 24 ) ^ reverse_32(crc_reg);
				break;
			default:
				break;
		}
	}

	return ~crc;
}

////=============================================================================
///*
//*	Not used
//*/
//static uint32_t crc32hw_byte(uint8_t byte, int clear)
//{	
//	uint32_t crc;	
//	uint32_t crc_reg = 0xffffffff;
//	
//	if(clear) CRC_ResetDR();
//	
//	crc = reverse_32(crc_reg);

//	CRC_CalcCRC(crc_reg);	//reset to 0	
//	
//	crc_reg = CRC_CalcCRC(reverse_32((byte & 0xFF) ^ crc) >> 24);
//	crc = ( crc >> 8 ) ^ reverse_32(crc_reg);

//	return ~crc;
//}

//=============================================================================
uint32_t crc32hw_append(uint32_t crc, const void *buf, uint32_t len)
{	
	uint8_t *p = (uint8_t *)buf;
	
	CRC_ResetDR();
	
	uint32_t crc_reg = 0xffffffff;
	
	crc = ~crc;
	
	for(uint32_t i = 0; i < len; i++)
	{
		CRC_CalcCRC(crc_reg);	//reset to 0
		uint32_t help = *p++;
		crc_reg = CRC_CalcCRC(reverse_32((help & 0xFF) ^ crc) >> 24);
		crc = ( crc >> 8 ) ^ reverse_32(crc_reg);
	}
	
	return ~crc;
}
