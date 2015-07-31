#ifndef _NRFRADIO_H
#define _NRFRADIO_H

#include "stm32f4xx.h"

#define SPIx									SPI1
#define SPI_PORT							GPIOA
#define SPI_CS_Pin						9
#define SPI_SCK_Pin						5
#define SPI_MISO_Pin					6
#define SPI_MOSI_Pin					7

#define CHIPEN_PORT						GPIOA
#define CHIPEN_Pin						8

#define INT_PORT							GPIOC
#define INT_Pin								9

#define NRF_CMD_READREG				0x00
#define NRF_CMD_WRITEREG			0x20
#define NRF_CMD_WRITE_TX			0xa0

#define NRF_REG_CONFIG				0x00
#define NRF_REG_ENSHOCKBST		0x01
#define NRF_REG_ENRXADDR			0x02
#define NRF_REG_AWIDTH				0x03
#define NRF_REG_RETR					0x04
#define NRF_REG_CHANNEL				0x05
#define NRF_REG_RFSETUP				0x06
#define NRF_REG_STATUS				0x07
#define NRF_REG_OBSERVETX			0x08

#define NRF_REG_RXADDR_P0			0x0a
#define NRF_REG_RXADDR_P1			0x0b
#define NRF_REG_RXADDR_P2			0x0c
#define NRF_REG_RXADDR_P3			0x0d
#define NRF_REG_RXADDR_P4			0x0e
#define NRF_REG_RXADDR_P5			0x0f
#define NRF_REG_TXADDR				0x10
#define NRF_REG_RXPLEN_P0			0x11
#define NRF_REG_RXPLEN_P1			0x12
#define NRF_REG_RXPLEN_P2			0x13
#define NRF_REG_RXPLEN_P3			0x14
#define NRF_REG_RXPLEN_P4			0x15
#define NRF_REG_RXPLEN_P5			0x16

#define RF_CFG_REG_VAL				0x4b

#define NRF_Pipe0							((uint8_t)0x01)
#define NRF_Pipe1							((uint8_t)0x02)
#define NRF_Pipe2							((uint8_t)0x04)
#define NRF_Pipe3							((uint8_t)0x08)
#define NRF_Pipe4							((uint8_t)0x10)
#define NRF_Pipe5							((uint8_t)0x20)

typedef enum PA_CFG
{
	PALevel_m18dBm,
	PALevel_m12dBm,
	PALevel_m6dBm,
	PALevel_0dBm
} PA_CFG_t;

typedef enum ADDR_WIDTH
{
	AddressWidth_3bytes = 1,
	AddressWidth_4bytes = 2,
	AddressWidth_5bytes = 3
} ADDR_WIDTH_t;

typedef enum RETR_WAIT
{
	RetransmissionDelay_250us,
	RetransmissionDelay_500us,
	RetransmissionDelay_750us,
	RetransmissionDelay_1000us,
	RetransmissionDelay_1250us,
	RetransmissionDelay_1500us,
	RetransmissionDelay_1750us,
	RetransmissionDelay_2000us,
	RetransmissionDelay_2250us,
	RetransmissionDelay_2500us,
	RetransmissionDelay_2750us,
	RetransmissionDelay_3000us,
	RetransmissionDelay_3250us,
	RetransmissionDelay_3500us,
	RetransmissionDelay_3750us,
	RetransmissionDelay_4000us
} RETR_WAIT_t;

typedef enum RETR_COUNT
{
	RetransmissionCount_Disable,
	RetransmissionCount_1,
	RetransmissionCount_2,
	RetransmissionCount_3,
	RetransmissionCount_4,
	RetransmissionCount_5,
	RetransmissionCount_6,
	RetransmissionCount_7,
	RetransmissionCount_8,
	RetransmissionCount_9,
	RetransmissionCount_10,
	RetransmissionCount_11,
	RetransmissionCount_12,
	RetransmissionCount_13,
	RetransmissionCount_14,
	RetransmissionCount_15,
} RETR_COUNT_t;

typedef enum DataRate
{
	DataRate_1Mbps,
	DataRate_2Mbps
} DataRate_t;

typedef struct pipe {
	uint8_t pipeRxAddr[5];								// Адрес, на который настроен данный пайп. Все 5 байт актуальны только для нулевого и первого пайпа. У остальных меняется только младший байт. Остальное от первого пайпа
	uint8_t pipeRxLen;										// Сколько байт в посылках
	FunctionalState enable;								// Вкл/выкл пайп
} pipe_t;

typedef struct nrf24l01 {
	uint8_t channel;											// Номер канала может быть любым числом от 0 до 127. Центральная частота - 2400 + channel (MHz)
	PA_CFG_t PALevel;											// Мощность выхода трансивера. Четыре дискретных значения (16 мкВт, 63 мкВт, 250 мкВт, 1 мВт)
	ADDR_WIDTH_t AddressWidth;						// Размер адреса
	DataRate_t DataRate;									// Скорость передачи данных
	RETR_COUNT_t RetransmissionCount;			// Количество переотправок при ошибке
	RETR_WAIT_t RetransmissionDelay;			// Задержка перед переотправкой
	uint8_t TxAddress[5];									// Адрес, куда будем пулять
	pipe_t pipe[6];												// Структуры с пайпами
	uint8_t pipeMask;											
} nrf24l01_t;

void nrfInit (nrf24l01_t *iData);
uint8_t spiXmit (uint8_t byte);
void delay_us (uint16_t delay);
void delay_ms (uint16_t delay);
void nrfWriteReg (uint8_t addr, uint8_t value);
void nrfWriteRegMulti (uint8_t addr, uint8_t *data, uint8_t len);
uint8_t nrfReadReg (uint8_t addr);
void nrfReadRegMulti (uint8_t addr, uint8_t *data, uint8_t len);
void nrfWriteData (uint8_t cmd, uint8_t *data, uint8_t len);
void nrfReadData (uint8_t cmd, uint8_t *data, uint8_t len);
uint8_t nrfGetStatus (void);
void nrfSetChannel (uint8_t channel);
void nrfPipeConfig (pipe_t *pipe, uint8_t n);
void nrfSetTxAddress (uint8_t *addr);
void nrfSetMasterRxAddr (uint8_t *addr, uint8_t rxLen, uint8_t *mask);
void nrfPipeAdd (uint8_t pipe, uint8_t addr, uint8_t *mask);
void nrfPipeDel (uint8_t pipe, uint8_t *mask);
void nrfTransmiteData (uint8_t *data, uint8_t len);

#endif
