/*
Билинейная интерполяция кусочной функции z=f(x,y).

Модуль позволяет найти значение z для любых x и y кусочной функции z=f(x,y).

//----------------------------------------------------------------
f задаётся двумерной матрицей значений z[y_size][x_size] и двумя векторами значений x[x_size] и y[y_size]:

Условия корректно заданных данных:
	
	x_size >= 2
	y_sixe >= 2
	x1 < x2 < x3 ...
	y1 < y2 < x3 ...

Функция piecewise_3d_check_input_data проверяет условия корректно заданных данных и возвращает 1, если всё ОК.
	
Пример для x_size=3 и y_size=2:

x[3] 		= 	{x1, x2, x3};
y[2] 		= 	{y1, y2};
z[2][3] = {	{z1, z2, z3}, 
						{z4, z5, z6}};

			|x1	x2	x3
______|__________		
y1		|z1	z2	z3
y2		|z4	z5	z6

*/
//Журавлев ЕА

#ifndef _PIECEWISE_3D_H_
#define _PIECEWISE_3D_H_

//================================================================
typedef struct
{
	int16_t* x;	//[x_size]
	int16_t* y;	//[y_size]
	int16_t* z;	//[y_size][x_size]
	
	uint32_t x_size;
	uint32_t y_size;
	
	uint8_t error_fl;
	
} piecewise_3d_t;

//================================================================
float piecewise_3d_get_z(piecewise_3d_t * piecewise, float x, float y);
uint8_t piecewise_3d_check_input_data(piecewise_3d_t * unit);


#endif
