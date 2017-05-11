/*
 * I2C software (bitbang) serial interface  rev.0.01 12.09.2011
 * 
 *
 */

#ifndef _I2C_BITBANG_H_
#define _I2C_BITBANG_H_

//=============================================================================
// conf section: select I2C_FRQ & write SCL-SDA manipulation macro
//=============================================================================

#define I2C_BITBANG_FRQ   100000 // 400kHz or 100kHz

// STM32 BRANCH:
#define I2C_SCL_INIT()	pin_init(PIN_I2C_SCL) // gpio open drain mode
#define I2C_SDA_INIT()	pin_init(PIN_I2C_SDA) // gpio open drain mode

#define I2C_SCL(X)      do {if (X) pin_set(PIN_I2C_SCL); else pin_clr(PIN_I2C_SCL);}  while (0)
#define I2C_SDA(X)      do {if (X) pin_set(PIN_I2C_SDA); else pin_clr(PIN_I2C_SDA);}  while (0)

#define I2C_SCL_READ()  pin_test(PIN_I2C_SCL)
#define I2C_SDA_READ()  pin_test(PIN_I2C_SDA)

// AVR BRANCH:
/*
#define I2C_SCL_INIT()	do {Set_Input(PIN_I2C_SCL); Tri_State(PIN_I2C_SCL);}   while (0) // tri-state + ext pullup = open drain mode
#define I2C_SDA_INIT()	do {Set_Input(PIN_I2C_SDA); Tri_State(PIN_I2C_SDA);}   while (0) // tri-state + ext pullup = open drain mode 

#define I2C_SCL(X)      do {if (X) Set_Input(PIN_I2C_SCL); else Set_Output(PIN_I2C_SCL);}   while (0)  // with ext pullup: inp=1  out=0
#define I2C_SDA(X)      do {if (X) Set_Input(PIN_I2C_SDA); else Set_Output(PIN_I2C_SDA);}   while (0)  // with ext pullup: inp=1  out=0

#define I2C_SCL_READ()  Test_Bit(PIN_I2C_SCL)
#define I2C_SDA_READ()  Test_Bit(PIN_I2C_SDA)
*/

//=============================================================================
// read-only section
//=============================================================================

#define	I2C_ACK        0

void         i2c_init       (void);

void         i2c_start      (void);
void         i2c_stop       (void);
uint_fast8_t i2c_write_byte (uint_fast8_t x);        // write byte to I2C msb first, returns true if ACK
uint_fast8_t i2c_read_byte  (uint_fast8_t ack_bit);  // read  1 byte from I2C msb first, then writes ACK bit

uint_fast8_t i2c_write_block(const uint8_t* src, size_t qty); // writes bytes to bus w/o start/stop conditions 
void         i2c_read_block (uint8_t*       dst, size_t qty); // reads bytes from bus w/o start/stop conditions and send NACK (SDA=1) on last byte


#endif
