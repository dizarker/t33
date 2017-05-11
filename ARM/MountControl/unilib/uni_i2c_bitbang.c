#include "include.h"

#define	I2C_ACK	0

//=============================================================================
void uni_i2c_init(uni_i2c_bitbang_t* unit)
{	
	//SDA and SCL must be open drain + external pullup
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//SCL	
	GPIO_StructInit(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = unit->SCL_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(unit->SCL_GPIOx, &GPIO_InitStructure);
	
	//SDA	
	GPIO_StructInit(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = unit->SDA_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(unit->SDA_GPIOx, &GPIO_InitStructure);
	
	//halfbit_delay
	if (!unit->frq)
	{
		unit->frq = 400000;
	}
	unit->halfbit_delay_us = (1E6/(float)unit->frq)/2.0f;
}

//=============================================================================
static void uni_i2c_delay_halfbit(uni_i2c_bitbang_t* unit)
{
	//return;
	if (!unit->halfbit_delay_us)
	{
		return;
	}
	delay_us(unit->halfbit_delay_us);
} 

//=============================================================================
static uint_fast8_t uni_i2c_rd_sda(uni_i2c_bitbang_t* unit)  // read only aftre release sda
{
	return(GPIO_ReadInputDataBit(unit->SDA_GPIOx, unit->SDA_GPIO_Pin));
}

//=============================================================================
static uint_fast8_t uni_i2c_rd_scl(uni_i2c_bitbang_t* unit)
{
	return(GPIO_ReadInputDataBit(unit->SCL_GPIOx, unit->SCL_GPIO_Pin));
}

//=============================================================================
static void uni_i2c_wr_scl(uni_i2c_bitbang_t* unit, uint_fast8_t abit) 
{
	if (abit)
	{
		GPIO_SetBits(unit->SCL_GPIOx, unit->SCL_GPIO_Pin);
	}
	else
	{
		GPIO_ResetBits(unit->SCL_GPIOx, unit->SCL_GPIO_Pin);
	}
	
	while (abit	&& !uni_i2c_rd_scl(unit)) // waiting for slave free SCL line
	{
		;
	} 

	uni_i2c_delay_halfbit(unit);
}

//=============================================================================
static void uni_i2c_wr_sda(uni_i2c_bitbang_t* unit, uint_fast8_t abit)
{
	if (abit)
	{
		GPIO_SetBits(unit->SDA_GPIOx, unit->SDA_GPIO_Pin);
	}
	else
	{
		GPIO_ResetBits(unit->SDA_GPIOx, unit->SDA_GPIO_Pin);
	}

	uni_i2c_delay_halfbit(unit);
}

//=============================================================================
void uni_i2c_start(uni_i2c_bitbang_t* unit)
{
	uni_i2c_wr_scl(unit, 0);  // sda can only change when scl is low
	uni_i2c_wr_sda(unit, 1);  // prepare SDA level in case of repeated start

	uni_i2c_wr_scl(unit, 1);  // 1-0 transition on SDA while SCL=1
	uni_i2c_wr_sda(unit, 0);
	uni_i2c_wr_scl(unit, 0);
}

//=============================================================================
void uni_i2c_stop(uni_i2c_bitbang_t* unit)
{	
	uni_i2c_wr_sda(unit, 0);  // 0-1 transition on SDA while SCL=1
	uni_i2c_wr_scl(unit, 1);
	uni_i2c_wr_sda(unit, 1);
}
  
//=============================================================================
static void uni_i2c_wr_bit(uni_i2c_bitbang_t* unit, uint_fast8_t x)
{
	uni_i2c_wr_sda(unit, x);

	uni_i2c_wr_scl(unit, 1);
	uni_i2c_wr_scl(unit, 0);
}
  
//=============================================================================
static uint_fast8_t uni_i2c_rd_bit(uni_i2c_bitbang_t* unit)
{
	uint_fast8_t x;

	uni_i2c_wr_sda(unit, 1);    // release SDA on SCL low

	uni_i2c_wr_scl(unit, 1);

	x = (uni_i2c_rd_sda(unit)!=0); // read aftre release sda

	uni_i2c_wr_scl(unit, 0);

	return (x);
}

//=============================================================================
int uni_i2c_write_adress_read(uni_i2c_bitbang_t* unit, uint_fast8_t adr)
{
	adr <<= 1;
	adr |= 0x01;
	
	for (uint_fast8_t msk=(1<<7); msk>0; msk>>=1)
	{	
		uni_i2c_wr_bit(unit, adr & msk);
	}

	return (uni_i2c_rd_bit(unit)==I2C_ACK); // release SDA inside
}

//=============================================================================
int uni_i2c_write_adress_write(uni_i2c_bitbang_t* unit, uint_fast8_t adr)
{
	adr <<= 1;
	
	for (uint_fast8_t msk=(1<<7); msk>0; msk>>=1)
	{	
		uni_i2c_wr_bit(unit, adr & msk);
	}

	return (uni_i2c_rd_bit(unit)==I2C_ACK); // release SDA inside
}

//=============================================================================
int uni_i2c_write_byte(uni_i2c_bitbang_t* unit, uint_fast8_t x) // write byte to I2C msb first, returns true if ACK
{
	for (uint_fast8_t msk=(1<<7); msk>0; msk>>=1)
	{	
		uni_i2c_wr_bit(unit, x & msk);
	}

	return (uni_i2c_rd_bit(unit)==I2C_ACK); // release SDA inside
}
  
//=============================================================================
uint_fast8_t uni_i2c_read_byte(uni_i2c_bitbang_t* unit, uint_fast8_t ack_bit) // read byte from I2C msb first, then writes ACK bit
{
	uint_fast8_t x=0;

	for (uint_fast8_t msk=(1<<7); msk>0; msk>>=1)
	{
		if (uni_i2c_rd_bit(unit))  x |= msk;
	}

	uni_i2c_wr_bit(unit, ack_bit);

	return (x);
}

//=============================================================================
uint_fast8_t uni_i2c_write_block(uni_i2c_bitbang_t* unit, const uint8_t* src, size_t qty)    // writes bytes to bus w/o start/stop conditions 
{
	while (qty && uni_i2c_write_byte(unit, *src++))
	{
		qty--;
	}

	return (qty<=1); // NAK at last byte is normal
}

//=============================================================================
void uni_i2c_read_block(uni_i2c_bitbang_t* unit, uint8_t* dst, size_t qty) // reads bytes from bus w/o start/stop conditions and send NACK (1) on last byte
{
	while (qty--) 
	{
		*dst++ = uni_i2c_read_byte(unit, !qty);   // send NACK (1) on last byte
	}
}
