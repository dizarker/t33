#ifndef _NRFRADIO_H
#define _NRFRADIO_H

#include "stm32f4xx.h"

#define SPIx							SPI1
#define SPI_PORT						GPIOA
#define SPI_CS_Pin						9
#define SPI_SCK_Pin						5
#define SPI_MISO_Pin					6
#define SPI_MOSI_Pin					7

#define CHIPEN_PORT						GPIOC
#define CHIPEN_Pin						8

#define INT_PORT						GPIOC
#define INT_Pin							9

void nrfInit (void);
uint8_t spiXmit (uint8_t byte);
void _delay_us (void);

#endif
