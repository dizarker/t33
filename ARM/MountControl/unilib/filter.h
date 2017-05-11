/*
 *  filter.h rev.2.2.0 2010-07-08
 *
 *  Filters unit.
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 */

#ifndef _FILTER_H_
#define _FILTER_H_

#include <stdlib.h>
#include <inttypes.h>

# ifdef __cplusplus
extern "C" {
# endif

uint16_t median_filter (uint16_t* p, size_t n);                // destroys input buffer (arrange bytes)
float    exp_smoothing (float* tek, float* prev, float kexp);

	
//hi_pass_filter ЖуравлевЕА
/*
	как RC цепочка
	пример: частота поступления данных в фильтр 1 кГц
	для постоянной времени фильтра в 30 мс делаем filter_hi_pass_t.length = 30
*/
typedef struct
{
	volatile int32_t length;
	volatile int32_t data_sum;
	volatile int32_t data_out;
} filter_hi_pass_t;

void filter_hi_pass_init(filter_hi_pass_t * filter, int32_t data_in);
int32_t filter_hi_pass(filter_hi_pass_t * filter, int32_t data_in);

//FLOAT hi_pass_filter ЖуравлевЕА
/*
	как RC цепочка
	пример: частота поступления данных в фильтр 1 кГц
	для постоянной времени фильтра в 30 мс делаем filter_hi_pass_t.length = 30
*/

typedef struct
{
	int32_t length;
	float data_sum;
	float data_out;
} filter_hi_pass_f_t;

void filter_hi_pass_f_init(filter_hi_pass_f_t * filter, float data_in);
int32_t filter_hi_pass_f(filter_hi_pass_f_t * filter, float data_in);

//среднее арифметическое ЖуравлевЕА
/*
	складывает data_in в data_sum, lenth раз, потом делит на lenth и помещает в data_out
*/
typedef struct
{
	uint32_t length;
	uint32_t data_cnt;
	uint32_t data_sum;
	uint32_t data_out;
}filter_arithmetic_mean_t;

int32_t filter_arithmetic_mean(filter_arithmetic_mean_t * filter, uint32_t data_in);

//filter_hi_pass_boost ЖуравлевЕА
/*
работает как RC цепочка НО:
	при резком увеличении (больше up_boost_delta) или резком уменьшении (меньше down_boost_delta) - фильтр даёт на выходе входное значение
	при up_boost_delta = 0 && down_boost_delta = 0 работает как обычный hi_pass_filter
*/
typedef struct
{
	int32_t length;
	int32_t data_sum;
	int32_t data_out;
	int32_t up_boost_delta;
	int32_t down_boost_delta;
}filter_hi_pass_boost_t;

int32_t filter_hi_pass_boost(filter_hi_pass_boost_t * filter, int32_t data_in);

//hi_pass_filter_init_fast ЖуравлевЕА
/*
	Работает так: пришло первое значение - оно же на выходе, пришло второе значение на выходе среднее из них и так до length значений, потом так же как hi_pass_filter
*/
typedef struct
{
	int32_t length;
	int32_t data_cnt;
	int32_t data_sum;
	int32_t data_out;
} filter_hi_pass_fast_start_t;

void filter_hi_pass_fast_start_reset(filter_hi_pass_fast_start_t * filter);
int32_t filter_hi_pass_fast_start(filter_hi_pass_fast_start_t * filter, int32_t data_in);

//hi_pass_filter_init_fast_f ЖуравлевЕА
/*
	FLOAT
	Работает так: пришло первое значение - оно же на выходе, пришло второе значение на выходе среднее из них и так до length значений, потом так же как hi_pass_filter
*/
typedef struct
{
	int32_t length;
	float data_cnt;
	float data_sum;
	float data_out;
} filter_hi_pass_fast_start_f_t;

void filter_hi_pass_fast_start_f_reset(filter_hi_pass_fast_start_f_t * filter);
float filter_hi_pass_fast_start_f(filter_hi_pass_fast_start_f_t * filter, float data_in);	

/*
*	Need array 
*/
float moving_average_filter(float *src, float new_sense, uint32_t length);

# ifdef __cplusplus
}
# endif

#endif


