//Билинейная интерполяция кусочной функции z=f(x,y).
//Журавлев ЕА

#include  "include.h"

//#define LINEAR_DEPEND_Y_F(x, x1, x2, y1, y2) ((((float)x-x1)/((float)x2-x1))*((float)y2-y1) + y1)

//================================================================
void piecewise_3d_find_cnt1_cnt2(float val, uint32_t *cnt1, uint32_t* cnt2, int16_t* mass, uint32_t size)
{
	uint32_t i;
	
	if (val < mass[0])
	{
		*cnt1 = 0;
		*cnt2 = 1;
	}
	else if (val >= mass[size - 1])
	{
		*cnt1 = size - 2;
		*cnt2 = size - 1;
	}
	else
	{
		for (i = 0; i < (size - 1); i++)
		{
			if ((val >= mass[i]) && (val < mass[i + 1]))
			{
				*cnt1 = i;
				*cnt2 = i + 1;
				break;
			}
		}
	}
}

//================================================================
float piecewise_3d_get_z(piecewise_3d_t * unit, float x, float y)
{	
	uint32_t x1_cnt, x2_cnt, y1_cnt, y2_cnt, z1_cnt, z2_cnt;
	float r1, r2;
	float z;
	
	//Ищем 2 точки X
	piecewise_3d_find_cnt1_cnt2(x, &x1_cnt, &x2_cnt, unit->x, unit->x_size);
	
	//Ищем 2 точки Y
	piecewise_3d_find_cnt1_cnt2(y, &y1_cnt, &y2_cnt, unit->y, unit->y_size);
	
	//Ищем промежуточные значения r1 и r2
	z1_cnt = y1_cnt * unit->x_size + x1_cnt;
	z2_cnt = y1_cnt * unit->x_size + x2_cnt;
	r1 = LINEAR_DEPEND_Y_F(x, unit->x[x1_cnt], unit->x[x2_cnt], unit->z[z1_cnt], unit->z[z2_cnt]);
	
	z1_cnt = y2_cnt * unit->x_size + x1_cnt;
	z2_cnt = y2_cnt * unit->x_size + x2_cnt;
	r2 = LINEAR_DEPEND_Y_F(x, unit->x[x1_cnt], unit->x[x2_cnt], unit->z[z1_cnt], unit->z[z2_cnt]);
	
	//Ищем значение Z
	z  = LINEAR_DEPEND_Y_F(y, unit->y[y1_cnt], unit->y[y2_cnt], r1, r2);
	
	return z;
}

//================================================================
uint8_t piecewise_3d_check_input_data(piecewise_3d_t * unit)
{
	uint32_t i;
	
	if (unit->x_size < 2 || unit->y_size < 2)
	{
		unit->error_fl = 1;
	}
		
	for(i = 1; i < unit->x_size; i++)
	{
		if (unit->x[i - 1] >= unit->x[i])
		{
			unit->error_fl = 1;
		}
	}
	
	for(i = 1; i < unit->y_size; i++)
	{
		if (unit->y[i - 1] >= unit->y[i])
		{
			unit->error_fl = 1;
		}
	}
	
	return (!unit->error_fl);
}
