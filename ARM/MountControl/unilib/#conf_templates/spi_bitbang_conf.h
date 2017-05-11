#ifndef _SPI_BITBANG_CONF_H_
#define _SPI_BITBANG_CONF_H_

#define SPI_SCK(X)      do {if (X) pin_set(PIN_SPI_SCK);   else pin_clr(PIN_SPI_SCK);}   while (0)
#define SPI_MOSI(X)     do {if (X) pin_set(PIN_SPI_MOSI);  else pin_clr(PIN_SPI_MOSI);}  while (0)
#define SPI_MISO()      pin_test(PIN_SPI_MISO)


#define SPI_VN100_CS(X) do {if (X) pin_set(PIN_VN100_CS);  else pin_clr(PIN_VN100_CS);}  while (0)
//#define SPI_MEM_CS(X)   do {if (X) pin_set(PIN_MEM_CS);    else pin_clr(PIN_MEM_CS);}    while (0)


//=============================================================================
// select one devise of:
//   SPI_VN100
//   SPI_MEM
//=============================================================================
#define spi_select(SPI_DEVICE) \
do                             \
{                              \
	SPI_DEVICE##_CS(0);        \
}                              \
while (0)                      \

//=============================================================================
// deselect one devise of:
//   SPI_VN100
//   SPI_MEM
//=============================================================================
#define spi_deselect(SPI_DEVICE) \
do                               \
{                                \
	SPI_DEVICE##_CS(1);          \
}                                \
while (0)                        \


//=============================================================================
// deselect all devices
//=============================================================================
static __inline void spi_deselect_all(void)
{
	spi_deselect(SPI_VN100);
}

//=============================================================================
static __inline void spi_delay(void)	  // 50 ns
{
	nop();
	nop();
	nop();
	nop();
}


#endif
