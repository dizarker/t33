/* 
 * STM32 USARTs module with DMA
 * DMA has total 12 channels (dma1 has 7ch and dma2 has 5ch)
 *
 * 1. The DMA2 controller is available only in high-density and XL-density devices.
 * 2. ADC3, SPI/I2S3, UART4, SDIO, TIM5, TIM6, DAC, TIM7, TIM8 DMA requests are available only in high-density devices
 *
 *
 * Table 78 & 79 in TRM. Summary of USART_DMA requests for each channel:
 * DMA_CH:  DMA1_CH2   DMA1_CH3   DMA1_CH4   DMA1_CH5   DMA1_CH6   DMA1_CH7   DMA2_CH3  DMA2_CH5
 * USART:   USART3_TX  USART3_RX  USART1_TX  USART1_RX  USART2_RX  USART2_TX  UART4_RX  UART4_TX 
 *
 */

#ifndef _UART_DMA_H_
#define _UART_DMA_H_

#include "uart_conf.h"

typedef struct
{
	#if F1_CHECK
		DMA_Channel_TypeDef* const sfr;
	#else
		DMA_Stream_TypeDef* const sfr;
		uint32_t DMA_Channel;
	#endif
	volatile uint8_t* const    buf;
	const size_t               size;       
	uint_fast16_t              idx;       
} dma_t;

typedef struct
{
	USART_TypeDef* const sfr;

	struct
	{
		dma_t rx;
//		dma_t tx;
	} dma;

	struct
	{
//		fifo_t rx;
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

void putchar6(char c);
char getchar6(void);


uint32_t 		uart_transmission_complete	(uart_t* const uart);
void 			uart_set_mode_tx			(uart_t* const uart, uint8_t status);
void 			uart_set_mode_rx			(uart_t* const uart, uint8_t status);
#endif
