
#ifndef _OVERSAMPLING_H_
#define _OVERSAMPLING_H_

//================================================================
/*
Оверсемплинг данных с АЦП.
Увеличиваем разрешение АЦП за счёт уменьшения полосы измерения (уменьшение частоты получения измерений).

//1. Создаём структурку

oversampling_t oversampling;

//2. Указываем на сколько БИТ хотим увеличить разрешение АЦП. 
//Внимание! Разрешение увеличится, но шум может остаться. Оверсемплинг корректно работает если у Вас шум имеет нормальное распределение Гаусса.

oversampling.settings_resolution_up = 4;
oversampling_init(&oversampling);

//3. Считываем показания с АЦП и запихиваем в функцию оверсемпленга.

uint16_t adc_cnt;
adc_cnt = GetAdcCnt();
while (!oversampling(&oversampling, adc_cnt))
{
	adc_cnt = GetAdcCnt();
}

//4. Как только функция oversampling() выдала 1 можно забрать данные в микро вольтах.
//Чем больше увеличение разрядности (settings_resolution_up) - тем больше данных (adc_cnt) нужно запихнуть в oversampling(), чтобы получить результат.

uint32_t adc_uv;
adc_uv = oversampling.external_data_out_uv;

*/

#define OVERSAMPLING_ADC_MAX_CNT 0xFFF //Максимальное значение АЦП ммикроконтроллера.
#define OVERSAMPLING_ADC_MAX_UV (uint32_t)3300000 //Опорное напряжение АЦП в микро вольтах.

//================================================================
typedef struct
{
	int32_t settings_resolution_up;
	
	int32_t internal_cnt;
	int32_t internal_cnt_max;
	int32_t internal_sum;
	int32_t internal_adc_cnt_max;
	int32_t internal_adc_max_uv_div_adc_cnt_max;
	
	int32_t external_data_out_uv;
	
	uint8_t external_complete_fl;
} oversampling_t;

//================================================================
uint8_t oversampling(oversampling_t* unit, int16_t data);
void oversampling_init(oversampling_t* unit);
void oversampling_reset(oversampling_t* unit);

#endif
