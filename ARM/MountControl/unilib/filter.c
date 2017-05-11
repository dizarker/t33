#include "filter.h"

//=============================================================================
static void bubble_sort(uint16_t* p, size_t n)
{
	if (n<2) return;
    
	for (size_t i=0; i<n; i++)
	{
		for (size_t j=1; j<(n-i); j++)
		{
			if( p[j-1]>p[j] ) 
			{
				uint16_t temp = p[j-1];
				p[j-1] = p[j];
				p[j] = temp;
			}
		}
	}
}

//=============================================================================
uint16_t median_filter(uint16_t* p, size_t n)
{
	if (!n)  return (0);

	bubble_sort(p, n);

	size_t i = n/2;

	if (n&1)  return (p[i]);                      // ����� ��������� ������� ��������.
	else      return (((uint32_t)p[i-1]+p[i])/2); // ����� ��������� ������� ������.
}

//=============================================================================
float exp_smoothing(float* tek, float* prev, float kexp)
{
//	float rezult = (*tek)*kexp + (1-kexp)*(*prev);
//	*prev = rezult;

	return (*prev += kexp*(*tek-*prev));
}

//Журавлев ЕА
//=============================================================================
void filter_hi_pass_init(filter_hi_pass_t * filter, int32_t data_in)
{
	filter->data_sum = data_in * filter->length;
	filter->data_out = data_in;
}

//=============================================================================
int32_t filter_hi_pass(filter_hi_pass_t * filter, int32_t data_in)
{
	filter->data_sum = filter->data_sum + data_in - filter->data_out;
	filter->data_out = filter->data_sum / filter->length;
	return (filter->data_out);
}

//Журавлев ЕА
//=============================================================================
void filter_hi_pass_f_init(filter_hi_pass_f_t * filter, float data_in)
{
	filter->data_sum = data_in * filter->length;
	filter->data_out = data_in;
}

//=============================================================================
int32_t filter_hi_pass_f(filter_hi_pass_f_t * filter, float data_in)
{
	filter->data_sum = filter->data_sum + data_in - filter->data_out;
	filter->data_out = filter->data_sum / filter->length;
	return (filter->data_out);
}

//Журавлев ЕА
//=============================================================================
//Среднее арифметическое
int32_t filter_arithmetic_mean(filter_arithmetic_mean_t * filter, uint32_t data_in)
{
	filter->data_sum += data_in;
	filter->data_cnt++;
	
	if (filter->data_cnt > (filter->length - 1))
	{
		filter->data_out = filter->data_sum / filter->length;
		filter->data_sum = 0;
		filter->data_cnt = 0;
	}
	
	return (filter->data_out);
}

//Журавлев ЕА
//=============================================================================
int32_t filter_hi_pass_boost(filter_hi_pass_boost_t * filter, int32_t data_in)
{
	if (filter->up_boost_delta && (data_in - filter->data_out) >  filter->up_boost_delta) //скачок вверх
	{
		filter->data_sum = data_in * filter->length;
	}
	else if (filter->down_boost_delta && (filter->data_out - data_in) >  filter->down_boost_delta) //скачок вниз
	{
		filter->data_sum = data_in * filter->length;
	}
	else //нет скачков или выключено их детектирование
	{
		filter->data_sum = filter->data_sum + data_in - filter->data_out;
	}	
	
	filter->data_out = filter->data_sum / filter->length;
	
	return (filter->data_out);
}

//Журавлев ЕА
//=============================================================================
void filter_hi_pass_fast_start_reset(filter_hi_pass_fast_start_t * filter)
{
	filter->data_sum = filter->data_out;
	filter->data_cnt = 1;
}

//=============================================================================
int32_t filter_hi_pass_fast_start(filter_hi_pass_fast_start_t * filter, int32_t data_in)
{
	if (filter->data_cnt >= filter->length)
	{
		filter->data_sum = filter->data_sum + data_in - filter->data_out;
		filter->data_out = filter->data_sum / filter->length;
	}
	else
	{
		filter->data_sum = filter->data_sum + data_in;
		filter->data_cnt++;
		filter->data_out = filter->data_sum / filter->data_cnt;
	}
	
	return (filter->data_out);
}

//Журавлев ЕА
//=============================================================================
void filter_hi_pass_fast_start_f_reset(filter_hi_pass_fast_start_f_t * filter)
{
	filter->data_sum = filter->data_out;
	filter->data_cnt = 1;
}

//=============================================================================
float filter_hi_pass_fast_start_f(filter_hi_pass_fast_start_f_t * filter, float data_in)
{
	if (filter->data_cnt >= filter->length)
	{
		filter->data_sum = filter->data_sum + data_in - filter->data_out;
		filter->data_out = filter->data_sum / filter->length;
	}
	else
	{
		filter->data_sum = filter->data_sum + data_in;
		filter->data_cnt++;
		filter->data_out = filter->data_sum / filter->data_cnt;
	}
	
	return (filter->data_out);
}

//=============================================================================
float moving_average_filter(float *src, float new_sense, uint32_t length)
{
	static uint32_t qty 			= 0;
	static uint32_t already_do 		= 0;
	static float 	summ 			= 0;
	float 			average_sense	= 0;
		
	if(already_do)
	{
		summ -= src[qty];
	}
	
	src[qty++] = new_sense;
	
	if(already_do)
	{
		summ += new_sense;
		average_sense = summ/length;
	}
	
	if(qty >= length)
	{		
		if(!already_do)
		{
			for(int i = 0; i < length; i++)
			{
				summ += src[i];
			}
			average_sense = summ/length;
			already_do = 1;
		}
		
		qty = 0;
	}
	if(!already_do)
	{
		return new_sense;
	}

	return average_sense;
}