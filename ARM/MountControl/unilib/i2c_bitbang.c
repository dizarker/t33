#include "include.h"

//=============================================================================
void i2c_init(void)
{	
	I2C_SCL_INIT(); // must be open drain + external pullup
	I2C_SDA_INIT(); // must be open drain + external pullup
}

//=============================================================================
static void i2c_delay_halfbit(void)
{	
	delay_us((1E6/(float)I2C_BITBANG_FRQ)/2.0f);
} 

//=============================================================================
static uint_fast8_t i2c_rd_sda(void)  // read only aftre release sda
{
	return (I2C_SDA_READ());	
}

//=============================================================================
static uint_fast8_t i2c_rd_scl(void)
{
	return (I2C_SCL_READ());	
}

//=============================================================================
static void i2c_wr_scl(uint_fast8_t abit) 
{
	I2C_SCL(abit);

	while (abit	&& !i2c_rd_scl()) // waiting for slave free SCL line
	{
		;
	} 

	i2c_delay_halfbit();
}

//=============================================================================
static void i2c_wr_sda(uint_fast8_t abit)
{
	I2C_SDA(abit);

	i2c_delay_halfbit();
}

//=============================================================================
void i2c_start(void)
{
	dprintf("start\n");


	i2c_wr_scl(0);  // sda can only change when scl is low
	i2c_wr_sda(1);  // prepare SDA level in case of repeated start
 	
   	i2c_wr_scl(1);  // 1-0 transition on SDA while SCL=1
   	i2c_wr_sda(0);
   	i2c_wr_scl(0);
}

//=============================================================================
void i2c_stop(void)
{	
	dprintf("stop\n");

   	i2c_wr_sda(0);  // 0-1 transition on SDA while SCL=1
   	i2c_wr_scl(1);
   	i2c_wr_sda(1);
}
  
//=============================================================================
static void i2c_wr_bit(uint_fast8_t x)
{
   	i2c_wr_sda(x);

   	i2c_wr_scl(1);
   	i2c_wr_scl(0);
}
  
//=============================================================================
static uint_fast8_t i2c_rd_bit(void)
{
	uint_fast8_t x;

	i2c_wr_sda(1);    // release SDA on SCL low

   	i2c_wr_scl(1);

   	x = (i2c_rd_sda()!=0); // read aftre release sda

   	i2c_wr_scl(0);

   	return (x);
}
  
//=============================================================================
int i2c_write_byte(uint_fast8_t x) // write byte to I2C msb first, returns true if ACK
{
  	for (uint_fast8_t msk=(1<<7); msk>0; msk>>=1)
   	{	
		i2c_wr_bit(x & msk);
   	}

   	return (i2c_rd_bit()==I2C_ACK); // release SDA inside
}
  
//=============================================================================
uint_fast8_t i2c_read_byte(uint_fast8_t ack_bit) // read byte from I2C msb first, then writes ACK bit
{
	uint_fast8_t x=0;

   	for (uint_fast8_t msk=(1<<7); msk>0; msk>>=1)
   	{
    	if (i2c_rd_bit())  x |= msk;
   	}

   	i2c_wr_bit(ack_bit);

   	return (x);
}

//=============================================================================
uint_fast8_t i2c_write_block(const uint8_t* src, size_t qty)    // writes bytes to bus w/o start/stop conditions 
{
	dprintf("\ni2c_wr_block:%d\n", qty);

	while (qty && i2c_write_byte(*src++))
	{
		dprintx(src[-1]);

		qty--;
	}

	return (qty<=1); // NAK at last byte is normal
}

//=============================================================================
void i2c_read_block(uint8_t* dst, size_t qty) // reads bytes from bus w/o start/stop conditions and send NACK (1) on last byte
{
	dprintf("\ni2c_rd_block:%d\n", qty);

	while (qty--) 
	{
		*dst++ = i2c_read_byte(!qty);   // send NACK (1) on last byte

		dprintx(dst[-1]);
	}
}
