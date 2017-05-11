#ifndef _RX_ALL_DATA_H_
#define _RX_ALL_DATA_H_

#define SIZE_MASS 512
typedef struct
{
	uint8_t mass[SIZE_MASS];
	uint32_t offset;
	uint32_t i;
	uint32_t size;
	uint32_t status;
	uint32_t stop_write;
}data_t;


//=============================================================================
#ifdef MAIN
uint32_t rx_all_data(uart_t* const uart);
data_t data;
void put_data(void);
//data.offset = 0;
#else
extern uint32_t rx_all_data(uart_t* const uart);
extern data_t data;
extern void put_data(void);
#endif
#endif
