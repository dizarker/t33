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
static INLINE void uart_isr(uart_t* const uart)
{
	uint_fast16_t status = uart->sfr->SR;

	if (status & USART_SR_TXE)  // if TX data_reg is empty (auto-clr by writing data_reg)
	{
		if (!fifo_get_byte(&uart->fifo.tx, (uint8_t*)&uart->sfr->DR))
		{
			uart_set_tx_int(uart, 0); // tx_int off
		}
	}
/*
	if (status & USART_SR_IDLE)  // IDLE isr ONLY (because of DMA)
	{  
		uart->sfr->DR;           // Auto-clr int-req by reading data_reg.
 	}
*/
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
	dma_t* const dma = &uart->dma.rx;
	int x;
	
	#if F1_CHECK
		if (dma->size - dma->sfr->CNDTR != dma->idx)
	#else
		if (dma->size - dma->sfr->NDTR != dma->idx)
	#endif
   {
		ENTER_CRITICAL_SECTION();
			x = dma->buf[dma->idx];
		LEAVE_CRITICAL_SECTION();

		if (++dma->idx >= dma->size)
		{
			dma->idx = 0;
		}

		return (x);
	}

	return (-1);
}

//=============================================================================
void uart_flush(uart_t* const uart)
{
//	uart->dma.rx.idx = 0;
//	fifo_flush(&uart->fifo.rx);
	fifo_flush(&uart->fifo.tx);
}

//=============================================================================
uint32_t uart_transmission_complete(uart_t* const uart)
{
	uint_fast16_t status = uart->sfr->SR;
	if(status & USART_SR_TC)
	{
		return 1;
	}
	return 0;
}

//=============================================================================
void uart_set_mode_rx(uart_t* const uart, uint8_t status)
{
	if(status)
	{
		uart->sfr->CR1 |= (1<<2);
	}
	else
	{
		uart->sfr->CR1 &= ~ (1<<2);
	}
}

//=============================================================================
void uart_set_mode_tx(uart_t* const uart, uint8_t status)
{
	if(status)
	{
		uart->sfr->CR1 |= (1<<3);
	}
	else
	{
		uart->sfr->CR1 &= ~ (1<<3);
	}
}

//=============================================================================
static void uart_init_rcc(uart_t* const uart)
{
	#if F1_CHECK
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
	#else
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
		#if F1_CHECK && (defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) || defined(STM32F10X_XL) || defined(STM32F10X_CL))
			#ifdef UART4
				case UART4_BASE: NVIC_EnableIRQ(UART4_IRQn); break;
			#endif

			#ifdef UART5
				case UART5_BASE: NVIC_EnableIRQ(UART5_IRQn); break;
			#endif
				
			#ifdef USART6
				case USART6_BASE: NVIC_EnableIRQ(USART6_IRQn); break;
			#endif
		#endif
		default: return;
	}
}


//*******************************************************************
//***F1***F1***F1***F1***F1***F1***F1***F1***F1***F1***F1***F1***F1**
//*******************************************************************
#if F1_CHECK

	//=============================================================================
	static void uart_init_dma_ch(DMA_Channel_TypeDef* const dma,  DMA_InitTypeDef* DMA_InitStructure)
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		DMA_DeInit(dma);
		DMA_Init(dma, DMA_InitStructure);
		DMA_Cmd(dma, ENABLE);    

	//    DMA_ITConfig(dma, DMA_IT_TC, ENABLE);
	//    DMA_ITConfig(dma, DMA_IT_HT, ENABLE);
	}

	//=============================================================================
	static void uart_init_dma(uart_t* const uart)
	{
			USART_DMACmd(uart->sfr, /*USART_DMAReq_Tx |*/ USART_DMAReq_Rx, ENABLE);
		
			DMA_InitTypeDef DMA_InitStructure_rx =
		{
			.DMA_PeripheralBaseAddr = (uint32_t)&uart->sfr->DR,
			.DMA_MemoryBaseAddr     = (uint32_t)&uart->dma.rx.buf[0],
			.DMA_DIR                = DMA_DIR_PeripheralSRC,
			.DMA_BufferSize         = uart->dma.rx.size,
			.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
			.DMA_MemoryInc          = DMA_MemoryInc_Enable,
			.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
			.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
			.DMA_Mode               = DMA_Mode_Circular,
			.DMA_Priority           = DMA_Priority_High,
			.DMA_M2M                = DMA_M2M_Disable
		};

		uart_init_dma_ch(uart->dma.rx.sfr,  &DMA_InitStructure_rx);

	/*
			DMA_InitTypeDef DMA_InitStructure_tx =
		{
			.DMA_PeripheralBaseAddr = (uint32_t)&uart->sfr->DR,
			.DMA_MemoryBaseAddr     = (uint32_t)&uart->dma.tx.buf[0],
			.DMA_DIR                = DMA_DIR_PeripheralSRC,
			.DMA_BufferSize         = uart->dma.tx.size,
			.DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
			.DMA_MemoryInc          = DMA_MemoryInc_Enable,
			.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
			.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
			.DMA_Mode               = DMA_Mode_Circular,
			.DMA_Priority           = DMA_Priority_High,
			.DMA_M2M                = DMA_M2M_Disable
		};

		uart_init_dma_ch(uart->dma.tx.sfr,  &DMA_InitStructure_tx);
	*/
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

		uart_init_dma(uart);

		USART_Cmd(uart->sfr, ENABLE);

		uart_init_nvic(uart); // enable irq
	}


	#define USART1_RX_DMA_CH           DMA1_Channel5             // according to TRM (Tables:78,79)
	#define USART1_RX_DMA_IRQHandler   DMA1_Channel5_IRQHandler
	#define USART1_TX_DMA_CH           DMA1_Channel4
	#define USART1_TX_DMA_IRQHandler   DMA1_Channel4_IRQHandler


	#define USART2_RX_DMA_CH           DMA1_Channel6
	#define USART2_RX_DMA_IRQHandler   DMA1_Channel6_IRQHandler
	#define USART2_TX_DMA_CH           DMA1_Channel7
	#define USART2_TX_DMA_IRQHandler   DMA1_Channel7_IRQHandler


	#define USART3_RX_DMA_CH           DMA1_Channel3
	#define USART3_RX_DMA_IRQHandler   DMA1_Channel3_IRQHandler
	#define USART3_TX_DMA_CH           DMA1_Channel2
	#define USART3_TX_DMA_IRQHandler   DMA1_Channel2_IRQHandler

	#define USART4_RX_DMA_CH           DMA2_Channel3
	#define USART4_RX_DMA_IRQHandler   DMA2_Channel3_IRQHandler
	#define USART4_TX_DMA_CH           DMA2_Channel5
	#define USART4_TX_DMA_IRQHandler   DMA2_Channel5_IRQHandler

	//void USART##N##_TX_DMA_IRQHandler(void) {uart_tx_dma_isr(&uart##N);}
	//void USART##N##_RX_DMA_IRQHandler(void) {;}


	//=============================================================================
	#define UART_ASSIGN(N, TX_BUFFER_SIZE, RX_BUFFER_SIZE)                   \
																																					 \
		void __irq USART##N##_IRQHandler(void)  {uart_isr(&uart##N);}        \
																																					 \
		void putchar##N##(char c)         {uart_putc(&uart##N, c);}          \
		char getchar##N##(void)           {return (uart_getc(&uart##N));}    \
																																				 \
		static volatile uint8_t rx##N##_buffer[MAX(1,RX_BUFFER_SIZE)];       \
		static volatile uint8_t tx##N##_buffer[MAX(1,TX_BUFFER_SIZE)];       \
																																				 \
		uart_t uart##N =                                                     \
		{                                                                    \
			(USART_TypeDef*)USART##N##_BASE,                                 \
																																		 \
			{/*dma*/                                                         \
				{                                                            \
					USART##N##_RX_DMA_CH,                                    \
					&rx##N##_buffer[0],                                      \
					MAX(1,RX_BUFFER_SIZE)                                    \
				}                                                            \
			},                                                               \
																																			 \
			{/*fifo*/                                                        \
				{                                                            \
					&tx##N##_buffer[0],                                      \
					MAX(1,TX_BUFFER_SIZE),                                   \
					0,                                                       \
					0                                                        \
				}                                                            \
			}                                                                \
		}

	//=============================================================================
	#if defined(UART1_TX_BUFFER_SIZE) && defined(UART1_RX_BUFFER_SIZE) && ((UART1_TX_BUFFER_SIZE>0) || (UART1_RX_BUFFER_SIZE>0))
	UART_ASSIGN(1, UART1_TX_BUFFER_SIZE, UART1_RX_BUFFER_SIZE);
	#endif

	//=============================================================================
	#if defined(UART2_TX_BUFFER_SIZE) && defined(UART2_RX_BUFFER_SIZE) && ((UART2_TX_BUFFER_SIZE>0) || (UART2_RX_BUFFER_SIZE>0))
	UART_ASSIGN(2, UART2_TX_BUFFER_SIZE, UART2_RX_BUFFER_SIZE);
	#endif

	//=============================================================================
	#if defined(UART3_TX_BUFFER_SIZE) && defined(UART3_RX_BUFFER_SIZE) && ((UART3_TX_BUFFER_SIZE>0) || (UART3_RX_BUFFER_SIZE>0))
	UART_ASSIGN(3, UART3_TX_BUFFER_SIZE, UART3_RX_BUFFER_SIZE);
	#endif

	//=============================================================================
	#if defined(UART4_TX_BUFFER_SIZE) && defined(UART4_RX_BUFFER_SIZE) && ((UART4_TX_BUFFER_SIZE>0) || (UART4_RX_BUFFER_SIZE>0))
	UART_ASSIGN(4, UART4_TX_BUFFER_SIZE, UART4_RX_BUFFER_SIZE);
	#endif

	//=============================================================================
	#if defined(UART5_TX_BUFFER_SIZE) && defined(UART5_RX_BUFFER_SIZE) && ((UART5_TX_BUFFER_SIZE>0) || (UART5_RX_BUFFER_SIZE>0))
	UART_ASSIGN(5, UART5_TX_BUFFER_SIZE, UART5_RX_BUFFER_SIZE);
	#endif


//*******************************************************************
//***F4***F4***F4***F4***F4***F4***F4***F4***F4***F4***F4***F4***F4**
//*******************************************************************
#else

	//=============================================================================
	static void uart_init_dma_ch(DMA_Stream_TypeDef* const dma,  DMA_InitTypeDef* DMA_InitStructure)
	{
		rcc_dma(DMA1, ENABLE);
		rcc_dma(DMA2, ENABLE);

		DMA_DeInit(dma);
		DMA_Init(dma, DMA_InitStructure);
		DMA_Cmd(dma, ENABLE);    

	//    DMA_ITConfig(dma, DMA_IT_TC, ENABLE);
	//    DMA_ITConfig(dma, DMA_IT_HT, ENABLE);
	}

	//=============================================================================
	static void uart_init_dma(uart_t* const uart)
	{
		USART_DMACmd(uart->sfr, /*USART_DMAReq_Tx |*/ USART_DMAReq_Rx, ENABLE);
		
		DMA_InitTypeDef DMA_InitStructure_rx;
		
		DMA_InitStructure_rx.DMA_Channel						= uart->dma.rx.DMA_Channel;
		DMA_InitStructure_rx.DMA_PeripheralBaseAddr = (uint32_t)&uart->sfr->DR;
		DMA_InitStructure_rx.DMA_Memory0BaseAddr    = (uint32_t)&uart->dma.rx.buf[0];
		DMA_InitStructure_rx.DMA_DIR                = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure_rx.DMA_BufferSize         = uart->dma.rx.size;
		DMA_InitStructure_rx.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
		DMA_InitStructure_rx.DMA_MemoryInc          = DMA_MemoryInc_Enable;
		DMA_InitStructure_rx.DMA_Mode               = DMA_Mode_Circular;
		DMA_InitStructure_rx.DMA_Priority           = DMA_Priority_High;
		DMA_InitStructure_rx.DMA_FIFOMode			= DMA_FIFOMode_Disable;
		DMA_InitStructure_rx.DMA_FIFOThreshold 		= DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure_rx.DMA_MemoryBurst 		= DMA_MemoryBurst_Single;
		DMA_InitStructure_rx.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;
		DMA_InitStructure_rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure_rx.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
		DMA_InitStructure_rx.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
		
		uart_init_dma_ch(uart->dma.rx.sfr,  &DMA_InitStructure_rx);
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

		uart_init_dma(uart);

		USART_Cmd(uart->sfr, ENABLE);

		uart_init_nvic(uart); // enable irq
	}

	//=============================================================================

	#define USART1_RX_DMA_STEAM        DMA2_Stream2
	#define USART1_RX_DMA_CH           DMA_Channel_4
	#define USART1_RX_DMA_IRQHandler   DMA2_Stream2_IRQHandler

	#define USART2_RX_DMA_STEAM        DMA1_Stream5
	#define USART2_RX_DMA_CH           DMA_Channel_4
	#define USART2_RX_DMA_IRQHandler   DMA1_Stream5_IRQHandler

	#define USART3_RX_DMA_STEAM        DMA1_Stream1
	#define USART3_RX_DMA_CH           DMA_Channel_4
	#define USART3_RX_DMA_IRQHandler   DMA1_Stream1_IRQHandler
	
	#define UART4_RX_DMA_STEAM      	 DMA1_Stream2
	#define UART4_RX_DMA_CH            DMA_Channel_4
	#define UART4_RX_DMA_IRQHandler    DMA1_Stream2_IRQHandler
	
	#define UART5_RX_DMA_STEAM         DMA1_Stream0
	#define UART5_RX_DMA_CH            DMA_Channel_4
	#define UART5_RX_DMA_IRQHandler    DMA1_Stream0_IRQHandler

	#define USART6_RX_DMA_STEAM        DMA2_Stream1
	#define USART6_RX_DMA_CH           DMA_Channel_5
	#define USART6_RX_DMA_IRQHandler   DMA2_Stream1_IRQHandler


	//=============================================================================
	#define USART_ASSIGN(N, TX_BUFFER_SIZE, RX_BUFFER_SIZE)                  \
																			 \
		void __irq USART##N##_IRQHandler(void)  {uart_isr(&uart##N);}       \
																			 \
		void putchar##N##(char c)         {uart_putc(&uart##N, c);}          \
		char getchar##N##(void)           {return (uart_getc(&uart##N));}    \
																			 \
		static volatile uint8_t rx##N##_buffer[MAX(1,RX_BUFFER_SIZE)];       \
		static volatile uint8_t tx##N##_buffer[MAX(1,TX_BUFFER_SIZE)];       \
																			 \
		uart_t uart##N =                                                     \
		{                                                                    \
			(USART_TypeDef*)USART##N##_BASE,                                 \
																			 \
			{/*dma*/                                                         \
				{                                                            \
					USART##N##_RX_DMA_STEAM,								 \
					USART##N##_RX_DMA_CH,									 \
					&rx##N##_buffer[0],                                      \
					MAX(1,RX_BUFFER_SIZE)                                    \
				}                                                            \
			},                                                               \
																			 \
			{/*fifo*/                                                        \
				{                                                            \
					&tx##N##_buffer[0],                                      \
					MAX(1,TX_BUFFER_SIZE),                                   \
					0,                                                       \
					0                                                        \
				}                                                            \
			}                                                                \
		}                                                                    
		                                                                     
	//=============================================================================
	#define UART_ASSIGN(N, TX_BUFFER_SIZE, RX_BUFFER_SIZE)                   \
																			 \
		void __irq UART##N##_IRQHandler(void)  {uart_isr(&uart##N);}        \
																			 \
		void putchar##N##(char c)         {uart_putc(&uart##N, c);}          \
		char getchar##N##(void)           {return (uart_getc(&uart##N));}    \
																			 \
		static volatile uint8_t rx##N##_buffer[MAX(1,RX_BUFFER_SIZE)];       \
		static volatile uint8_t tx##N##_buffer[MAX(1,TX_BUFFER_SIZE)];       \
																			 \
		uart_t uart##N =                                                     \
		{                                                                    \
			(USART_TypeDef*)UART##N##_BASE,                                  \
																			 \
			{/*dma*/                                                         \
				{                                                            \
					UART##N##_RX_DMA_STEAM,									 \
					UART##N##_RX_DMA_CH,									 \
					&rx##N##_buffer[0],                                      \
					MAX(1,RX_BUFFER_SIZE)                                    \
				}                                                            \
			},                                                               \
																			 \
			{/*fifo*/                                                        \
				{                                                            \
					&tx##N##_buffer[0],                                      \
					MAX(1,TX_BUFFER_SIZE),                                   \
					0,                                                       \
					0                                                        \
				}                                                            \
			}                                                                \
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




	
	
#endif

