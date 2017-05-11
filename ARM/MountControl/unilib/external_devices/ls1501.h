#ifndef _LS1501_H_
#define _LS1501_H_



void ls1501_continuous_mode_on(uart_t* const uart);

#pragma pack(push, 1)
typedef struct 
{	
	double  ditance;	
}ls1501_data_t;
#pragma pack(pop)

typedef struct 
{	
	unsigned char arr[10];
	unsigned char new_data;	
}ls1501_log_t;

extern ls1501_log_t ls1501_log;
extern timeout_t ls1501_reload;
int ls1501_task(uart_t* const uart);
void ls1501_init(uart_t* const uart);
void ls1501_continuous_distance_internal_on(uart_t* const uart);
#endif
