//Кусочно-линейная аппроксимация
//Журавлев ЕА

#include  "include.h"

//================================================================
piecewise_point_t * get_point_pt(piecewise_t * piecewise, uint32_t point_num)
{
	point_num--; //превращаем порядковый номер точки в сдвиг
	
	return (piecewise->f + point_num);
}

//================================================================
float find_y_on_line(piecewise_t * piecewise, uint32_t point_num_1, uint32_t point_num_2, float x)
{
	float y;
	piecewise_point_t point_1, point_2;
		
	point_1 = *(get_point_pt(piecewise, point_num_1));
	point_2 = *(get_point_pt(piecewise, point_num_2));
	
	if ((!point_1.x && !point_2.x) || (!(point_2.x - point_1.x))) //если предвидится деление на ноль
	{
		return 0;
	}
	
	y = LINEAR_DEPEND_Y_F(x, point_1.x, point_2.x, point_1.y, point_2.y);
	return y;
}

//================================================================
float get_x(piecewise_t * piecewise, uint32_t point_num)
{	
	float x;
	
	x = (*(get_point_pt(piecewise, point_num))).x;
	
	return (x);
}

//================================================================
float piecewise_get_y(piecewise_t * piecewise, float x)
{
	uint32_t i;

	if (piecewise->size < 2) //нужно как минимум две точки для задания функции
	{
		return -1;
	}
	else if (x < get_x(piecewise, 1)) //х < минимальной точки функции
	{
		return find_y_on_line(piecewise, 1, 2, x);
	}
	else if (x >= get_x(piecewise, piecewise->size)) //х > максимальной точки функции
	{		
		return find_y_on_line(piecewise, piecewise->size - 1, piecewise->size, x);
	}
	else	//ищим х внутри функции
	{
		for (i = 1; i < piecewise->size; i++)
		{
			if (x >= get_x(piecewise, i) && x < get_x(piecewise, i + 1))
			{
				return find_y_on_line(piecewise, i, i + 1, x); 
			}
		}
		return -1; //ошибка
	}
}

//================================================================
uint32_t piecewise_get_near_point_number(piecewise_t * piecewise, float x)
{
	uint32_t i;
	
	if (piecewise->size < 2) //нужно как минимум две точки для задания функции
	{
		return 1; //ошибка
	}
	if (x < get_x(piecewise, 1)) //х < минимальной точки функции
	{
		return 1;
	}
	else if (x >= get_x(piecewise, piecewise->size)) //х > максимальной точки функции
	{		
		return piecewise->size;
	}
	else	//ищим х внутри функции
	{
		for (i = 1; i < piecewise->size; i++)
		{
			if (x >= get_x(piecewise, i) && x < get_x(piecewise, i + 1))
			{
				if ((x - get_x(piecewise, i)) < (get_x(piecewise, i + 1) - x))
				{
					return i;
				}
				else
				{
					return (i + 1);
				}
			}
		}
		return 1; //ошибка
	}
}

//================================================================
piecewise_point_t * piecewise_get_point(piecewise_t * piecewise, int32_t point_number)
{
	if (point_number < 1)
	{
		return (get_point_pt(piecewise, 1));
	}
	else if (point_number > piecewise->size)
	{
		return (get_point_pt(piecewise, piecewise->size));
	}
	else
	{
		return (get_point_pt(piecewise, point_number));
	}
}
