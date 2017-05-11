#ifndef _UNI_I2C_BITBANG_H_
#define _UNI_I2C_BITBANG_H_

//================================================================
/*
DEPENDENCIES
delay.c/.h
stm32f10x_gpio.c/h
*/

//================================================================
typedef struct
{
	uint32_t frq; // 400kHz or 100kHz
	uint32_t halfbit_delay_us;
	
	GPIO_TypeDef* SCL_GPIOx;
	uint16_t SCL_GPIO_Pin;
	
	GPIO_TypeDef* SDA_GPIOx;
	uint16_t SDA_GPIO_Pin;
} uni_i2c_bitbang_t;

//=============================================================================
void uni_i2c_init(uni_i2c_bitbang_t* unit);
void uni_i2c_start(uni_i2c_bitbang_t* unit);
void uni_i2c_stop(uni_i2c_bitbang_t* unit);

int uni_i2c_write_adress_read(uni_i2c_bitbang_t* unit, uint_fast8_t adr);
int uni_i2c_write_adress_write(uni_i2c_bitbang_t* unit, uint_fast8_t adr);

int uni_i2c_write_byte(uni_i2c_bitbang_t* unit, uint_fast8_t x); // write byte to I2C msb first, returns true if ACK
uint_fast8_t uni_i2c_read_byte(uni_i2c_bitbang_t* unit, uint_fast8_t ack_bit); // read byte from I2C msb first, then writes ACK bit

uint_fast8_t uni_i2c_write_block(uni_i2c_bitbang_t* unit, const uint8_t* src, size_t qty);    // writes bytes to bus w/o start/stop conditions 
void uni_i2c_read_block(uni_i2c_bitbang_t* unit, uint8_t* dst, size_t qty); // reads bytes from bus w/o start/stop conditions and send NACK (1) on last byte

#endif
