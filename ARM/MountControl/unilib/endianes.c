#include "include.h"

/*
 * todo: use on cm3 intrinsic: REV, REV16 and so on
 *
 */


//=============================================================================
uint_fast16_t read_word_lendian(const void* src_lendian)
{
	const uint8_t* src = src_lendian;

	return (((uint_fast16_t)src[1]<<8) | src[0]);
}

//=============================================================================
uint_fast16_t read_word_bendian(const void* src_bendian)
{
	const uint8_t* src = src_bendian;

	return (((uint_fast16_t)src[0]<<8) | src[1]);
}

//=============================================================================
void write_word_lendian(void* dst_lendian, uint_fast16_t x)
{
	uint8_t* dst = dst_lendian;

	dst[0] = (uint8_t)(x>>0); // lo
	dst[1] = (uint8_t)(x>>8); // hi
}

//=============================================================================
void write_word_bendian(void* dst_bendian, uint_fast16_t x)
{
	uint8_t* dst = dst_bendian;

	dst[0] = (uint8_t)(x>>8); // hi
	dst[1] = (uint8_t)(x>>0); // lo
}

//=============================================================================
uint32_t read_dword_lendian(const void* src_lendian)
{
	const uint8_t* src = src_lendian;

	uint32_t x = 0;
	
	int_fast8_t i = 4;

	while (i--)
	{
		x = (x<<8) | src[i];
	}

	return (x);
}

//=============================================================================
uint32_t read_dword_bendian(const void* src_bendian)
{
	const uint8_t* src = src_bendian;

	uint32_t x = 0;
	
	for (int_fast8_t i=0; i<4; i++)
	{
		x = (x<<8) | src[i];
	}

	return (x);
}

//=============================================================================
void write_dword_lendian(void* dst_lendian, uint32_t x)
{
	uint8_t* dst = dst_lendian;

	dst[0] = (uint8_t)(x>>0);  // lo
	dst[1] = (uint8_t)(x>>8);
	dst[2] = (uint8_t)(x>>16);
	dst[3] = (uint8_t)(x>>24); // hi
}


//=============================================================================
void write_dword_bendian(void* dst_bendian, uint32_t x)
{
	uint8_t* dst = dst_bendian;

	dst[0] = (uint8_t)(x>>24); // hi
	dst[1] = (uint8_t)(x>>16);
	dst[2] = (uint8_t)(x>>8);
	dst[3] = (uint8_t)(x>>0);  // lo
}

//=============================================================================
// float section:
//=============================================================================
STATIC_ASSERT(sizeof(float)==sizeof(uint32_t));

typedef union
{
	uint32_t dword;
	float    flt;
} dword_float_t;

//=============================================================================
float read_float_lendian(const void* src_lendian)
{
	dword_float_t x;

	x.dword = read_dword_lendian(src_lendian);

	return (x.flt);
}

//=============================================================================
float read_float_bendian(const void* src_bendian)
{
	dword_float_t x;

	x.dword = read_dword_bendian(src_bendian);

	return (x.flt);
}


//=============================================================================
void write_float_lendian(void* dst_lendian, float x)
{
	dword_float_t f;

	f.flt = x;

	write_dword_lendian(dst_lendian, f.dword);
}

//=============================================================================
void write_float_bendian(void* dst_bendian, float x)
{
	dword_float_t f;

	f.flt = x;

	write_dword_bendian(dst_bendian, f.dword);
}

