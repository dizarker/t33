/* 
 * STM32 USARTs w/o dma module  15.09.2012
 *
 *
 */

#ifndef _UART_H_
#define _UART_H_

#include "uart_conf.h"

typedef struct
{
	USART_TypeDef* const sfr;
	struct
	{
		fifo_t rx;
		fifo_t tx;
	} fifo;
} uart_t;

extern uart_t uart1;
extern uart_t uart2;
extern uart_t uart3;
extern uart_t uart4;
extern uart_t uart5;
extern uart_t uart6;

//=============================================================================
void uart_init      (uart_t* const uart, uint32_t br);  // br-8-n-1

void uart_putc      (uart_t* const uart, char c);
int  uart_getc      (uart_t* const uart);
void uart_flush     (uart_t* const uart);
void uart_put_block (uart_t* const uart, const void* src, size_t len);

void putchar1(char c);
char getchar1(void);

void putchar2(char c);
char getchar2(void);

void putchar3(char c);
char getchar3(void);

void putchar4(char c);
char getchar4(void);

void putchar5(char c);
char getchar5(void);

#endif
