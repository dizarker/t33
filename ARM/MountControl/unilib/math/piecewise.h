//Кусочно-линейная аппроксимация
//Журавлев ЕА

#ifndef _PIECEWISE_H_
#define _PIECEWISE_H_

//================================================================
typedef struct
{
	int16_t x;
	int16_t y;
} piecewise_point_t;

//================================================================
typedef struct
{
	piecewise_point_t * const 	f;
	uint32_t size;
} piecewise_t;

//================================================================
float piecewise_get_y(piecewise_t * piecewise, float x);
uint32_t piecewise_get_near_point_number(piecewise_t * piecewise, float x);
piecewise_point_t * piecewise_get_point(piecewise_t * piecewise, int32_t point_number);

#endif
