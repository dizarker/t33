#include "include.h"

//=============================================================================
static INLINE void uart_set_tx_int(uart_t* const uart, int x)
{
	if (x)
	{
		uart->sfr->CR1 |=  USART_CR1_TXEIE; // set TXEIE
	}
	else
	{
		uart->sfr->CR1 &= ~USART_CR1_TXEIE; // clr TXEIE
	}
}

//=============================================================================
static INLINE void uart_rx_isr(uart_t* const uart, uint_fast16_t status)
{
	static const uint_fast16_t ERR_MASK = USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE;	// overrun, nois or frame errors
		
	uint8_t data = uart->sfr->DR; // read data first
			
	if ((status & ERR_MASK)==0)   // if no errors
   	{
		fifo_put_byte(&uart->fifo.rx, data);
   	}
}

//=============================================================================
static INLINE void uart_tx_isr(uart_t* const uart)
{
	if (!fifo_get_byte(&uart->fifo.tx, (uint8_t*)&uart->sfr->DR))
	{
		uart_set_tx_int(uart, 0); // tx_int off
	}
}

//=============================================================================
static INLINE void uart_isr(uart_t* const uart)
{
	uint_fast16_t status = uart->sfr->SR;
		
	if (status & USART_SR_RXNE)      // if RX data_reg isn't empty (auto-clr by reading data_reg)
	{  
		uart_rx_isr(uart, status);
 	}
	else if (status & USART_SR_TXE)  // if TX data_reg is empty (auto-clr by writing data_reg)
	{
		uart_tx_isr(uart);
	}

}

//=============================================================================
void uart_putc(uart_t* const uart, char c)
{
	fifo_put_byte(&uart->fifo.tx, (uint8_t)c);
  
	uart_set_tx_int(uart, 1); // tx_int on
}


//=============================================================================
void uart_put_block(uart_t* const uart, const void* src, size_t len)
{
	if (len)
	{
		fifo_put_block(&uart->fifo.tx, src, len);

		uart_set_tx_int(uart, 1); // tx_int on
	}
}


//=============================================================================
int uart_getc(uart_t* const uart)
{
	uint8_t x;

	if (fifo_get_byte(&uart->fifo.rx, &x))
	{
		return (x);
	}

	return (-1);
}

//=============================================================================
void uart_flush(uart_t* const uart)
{
	fifo_flush(&uart->fifo.rx);
	fifo_flush(&uart->fifo.tx);
}


//=============================================================================
static void uart_init_rcc(uart_t* const uart)
{
	#if defined (STM32F10X_LD) || (STM32F10X_LD_VL) || (STM32F10X_MD) || (STM32F10X_MD_VL) || (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)
		switch ((uint32_t)uart->sfr)
		{
			case USART1_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); break;
			case USART2_BASE: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); break;
			case USART3_BASE: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); break;
		#ifdef USART4
			case USART4_BASE: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART4, ENABLE); break;
		#endif

		#ifdef USART5
			case USART5_BASE: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART5, ENABLE); break;
		#endif
			default: return;
		}

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // Enable AFIO clock
	
	
 	#elif defined (STM32F40_41xxx) || (STM32F427_437xx) || (STM32F429_439xx) || (STM32F401xx) || (STM32F411xE)
	
		switch ((uint32_t)uart->sfr)
		{
			case USART1_BASE: RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE); break;
			case USART2_BASE: RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN, ENABLE); break;
			case USART3_BASE: RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART3EN, ENABLE); break;
		#ifdef UART4
			case UART4_BASE: RCC_APB1PeriphClockCmd(RCC_APB1ENR_UART4EN, ENABLE); break;
		#endif

		#ifdef UART5
			case UART5_BASE: RCC_APB1PeriphClockCmd(RCC_APB1ENR_UART5EN, ENABLE); break;
		#endif
			
		#ifdef USART6
			case USART6_BASE: RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART6EN, ENABLE); break;
		#endif
			default: return;
		}

		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // Enable AFIO clock
		
	#endif	
	
}

//=============================================================================
static void uart_init_nvic(uart_t* const uart)
{
	switch ((uint32_t)uart->sfr)
	{
		case USART1_BASE: NVIC_EnableIRQ(USART1_IRQn); break;
		case USART2_BASE: NVIC_EnableIRQ(USART2_IRQn); break;
		case USART3_BASE: NVIC_EnableIRQ(USART3_IRQn); break;
	#ifdef UART4
		case UART4_BASE: NVIC_EnableIRQ(UART4_IRQn); break;
	#endif

	#ifdef UART5
		case UART5_BASE: NVIC_EnableIRQ(UART5_IRQn); break;
	#endif
		
	#ifdef USART6
		case USART6_BASE: NVIC_EnableIRQ(USART6_IRQn); break;
	#endif	
		default: return;
	}

	NVIC_SetPriority(USART1_IRQn, 2);
}


//=============================================================================
void uart_init(uart_t* const uart, uint32_t br)  // br-8-n-1
{
	USART_DeInit(uart->sfr);

	uart_init_rcc(uart);

	uart_flush(uart);

	USART_InitTypeDef USART_InitStruct =
	{
		.USART_BaudRate            = br,
		.USART_WordLength          = USART_WordLength_8b,
		.USART_StopBits            = USART_StopBits_1,
		.USART_Parity              = USART_Parity_No,
		.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None
	};

	USART_Init(uart->sfr, &USART_InitStruct);

	USART_ITConfig(uart->sfr, USART_IT_RXNE, ENABLE); // rx-interrupt only allowed by default

	USART_Cmd(uart->sfr, ENABLE);

	uart_init_nvic(uart); // enable irq
}

//=============================================================================
#define USART_ASSIGN(N, TX_BUFFER_SIZE, RX_BUFFER_SIZE)                \
                                                                      \
	void USART##N##_IRQHandler(void)  {uart_isr(&uart##N);}		     \
	void putchar##N##(char c)         {uart_putc(&uart##N, c);}       \
	char getchar##N##(void)           {return (uart_getc(&uart##N));} \
	                                                                  \
	static volatile uint8_t rx##N##_buffer[MAX(1,RX_BUFFER_SIZE)];    \
	static volatile uint8_t tx##N##_buffer[MAX(1,TX_BUFFER_SIZE)];    \
	                                                                  \
	uart_t uart##N =                                                  \
	{                                                                 \
		(USART_TypeDef*)USART##N##_BASE,                              \
		{                                                             \
			{                                                         \
				&rx##N##_buffer[0],                                   \
				MAX(1,RX_BUFFER_SIZE),                                \
				0,                                                    \
				0                                                     \
			},                                                        \
			{                                                         \
				&tx##N##_buffer[0],                                   \
				MAX(1,TX_BUFFER_SIZE),                                \
				0,                                                    \
				0                                                     \
			}                                                         \
		}                                                             \
	}

//=============================================================================
#define UART_ASSIGN(N, TX_BUFFER_SIZE, RX_BUFFER_SIZE)                \
                                                                      \
	void UART##N##_IRQHandler(void)  {uart_isr(&uart##N);}		     \
	void putchar##N##(char c)         {uart_putc(&uart##N, c);}       \
	char getchar##N##(void)           {return (uart_getc(&uart##N));} \
	                                                                  \
	static volatile uint8_t rx##N##_buffer[MAX(1,RX_BUFFER_SIZE)];    \
	static volatile uint8_t tx##N##_buffer[MAX(1,TX_BUFFER_SIZE)];    \
	                                                                  \
	uart_t uart##N =                                                  \
	{                                                                 \
		(USART_TypeDef*)UART##N##_BASE,                              \
		{                                                             \
			{                                                         \
				&rx##N##_buffer[0],                                   \
				MAX(1,RX_BUFFER_SIZE),                                \
				0,                                                    \
				0                                                     \
			},                                                        \
			{                                                         \
				&tx##N##_buffer[0],                                   \
				MAX(1,TX_BUFFER_SIZE),                                \
				0,                                                    \
				0                                                     \
			}                                                         \
		}                                                             \
	}
	
//=============================================================================
#if defined(UART1_TX_BUFFER_SIZE) && defined(UART1_RX_BUFFER_SIZE) && ((UART1_TX_BUFFER_SIZE>0) || (UART1_RX_BUFFER_SIZE>0))
USART_ASSIGN(1, UART1_TX_BUFFER_SIZE, UART1_RX_BUFFER_SIZE);
#endif

//=============================================================================
#if defined(UART2_TX_BUFFER_SIZE) && defined(UART2_RX_BUFFER_SIZE) && ((UART2_TX_BUFFER_SIZE>0) || (UART2_RX_BUFFER_SIZE>0))
USART_ASSIGN(2, UART2_TX_BUFFER_SIZE, UART2_RX_BUFFER_SIZE);
#endif

//=============================================================================
#if defined(UART3_TX_BUFFER_SIZE) && defined(UART3_RX_BUFFER_SIZE) && ((UART3_TX_BUFFER_SIZE>0) || (UART3_RX_BUFFER_SIZE>0))
USART_ASSIGN(3, UART3_TX_BUFFER_SIZE, UART3_RX_BUFFER_SIZE);
#endif

//=============================================================================
#if defined(UART4_TX_BUFFER_SIZE) && defined(UART4_RX_BUFFER_SIZE) && ((UART4_TX_BUFFER_SIZE>0) || (UART4_RX_BUFFER_SIZE>0))
UART_ASSIGN(4, UART4_TX_BUFFER_SIZE, UART4_RX_BUFFER_SIZE);
#endif

//=============================================================================
#if defined(UART5_TX_BUFFER_SIZE) && defined(UART5_RX_BUFFER_SIZE) && ((UART5_TX_BUFFER_SIZE>0) || (UART5_RX_BUFFER_SIZE>0))
UART_ASSIGN(5, UART5_TX_BUFFER_SIZE, UART5_RX_BUFFER_SIZE);
#endif

//=============================================================================
#if defined(UART6_TX_BUFFER_SIZE) && defined(UART6_RX_BUFFER_SIZE) && ((UART6_TX_BUFFER_SIZE>0) || (UART6_RX_BUFFER_SIZE>0))
USART_ASSIGN(6, UART6_TX_BUFFER_SIZE, UART6_RX_BUFFER_SIZE);
#endif
