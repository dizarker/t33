#ifndef _ILM150_T_H_
#define _ILM150_T_H_





#pragma pack(push, 1)
typedef struct 
{	
	double  ditance;	
}ilm150_t_data_t;
#pragma pack(pop)


int ilm150_t_task(uart_t* const uart);
void ilm150_t_init(void);

#endif
