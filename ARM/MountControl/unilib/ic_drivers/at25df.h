/*
 * SPI serial flash memory driver AT25DFxxx  rev.0.02 14.06.2012
 * 
 * SPI modes 1 or 3, msb first
 *
 * Flash has page's structure with auto increment wr/rd pointer INSIDE page.
 *
 * Power-up Device Delay Before Program or Erase Allowed is 10 ms.
 *
 */


#ifndef _AT25DF_H_
#define _AT25DF_H_

//=============================================================================
// conf section: select at25df_type & write CS and SPI macro
//=============================================================================

#define AT25DF_TYPE                         AT25DF321

#define at25df_spi_on()                     // sw spi is used
#define at25df_spi_off()

#define at25df_spi_cs_on()                  pin_clr(PIN_FLASH_SPI_CS)
#define at25df_spi_cs_off()                 pin_set(PIN_FLASH_SPI_CS)

/*
#define	at25df_spi_transfer(x)              spi_transfer(x)
#define	at25df_spi_transfer_block(buf,qty)  spi_transfer_block(buf,qty)
#define	at25df_spi_read_block(buf,qty)      spi_read_block(buf,qty)
#define	at25df_spi_write_block(buf,qty)     spi_write_block(buf,qty)
*/

//=============================================================================
// read-only section
//=============================================================================

#if (AT25DF_TYPE==AT25DF321)
#	define AT25DF_SIZE                0x400000UL  // 4MB (3-bytes pointer)
#	define AT25DF_PAGE_SIZE           256U
#	define AT25DF_PAGES_QTY           (AT25DF_SIZE/AT25DF_PAGE_SIZE)//-4

#	define AT25DF_SECTOR_SIZE         65536UL     // 64K
#	define AT25DF_SECTORS_QTY         ((AT25DF_SIZE)/AT25DF_SECTOR_SIZE)// - 4 * AT25DF_PAGE_SIZE

#	define AT25DF_BLOCK_4K_SIZE       4096U
#	define AT25DF_BLOCK_32K_SIZE      32768U
#	define AT25DF_BLOCK_64K_SIZE      65536UL


#	define AT25DF_ID                  0x1F470100UL
#else
#	error WRONG OR NOT YET SUPPORTED AT25DF_TYPE!!!
#endif

enum
{
	AT25DF_CMD_READ_ARRAY           = 0x0B,   // max 70MHz +1 don't care dummy byte
	AT25DF_CMD_READ_ARRAY_LOW_FRQ   = 0x03,   // max 33MHz w/o don't care dummy byte

	AT25DF_CMD_WRITE_ARRAY          = 0x02,   // 1.5-5 ms (write one byte time is 6us)

	AT25DF_CMD_ERASE_BLOCK4K        = 0x20,   // 50-200  ms   
	AT25DF_CMD_ERASE_BLOCK32K       = 0x52,   // 350-600 ms
	AT25DF_CMD_ERASE_BLOCK64K       = 0xD8,   // 600-950 ms

	AT25DF_CMD_ERASE_CHIP           = 0x60,   // 36-56 sec !!!
	AT25DF_CMD_ERASE_CHIP2          = 0xC7,   // the same as prev

	AT25DF_CMD_WRITE_ENABLE         = 0x06,
	AT25DF_CMD_WRITE_DISABLE        = 0x04,

	AT25DF_CMD_PROTECT_SECTOR       = 0x36,
	AT25DF_CMD_UNPROTECT_SECTOR     = 0x39,
	AT25DF_CMD_READ_PROTECTION_REGS = 0x3C,

	AT25DF_CMD_READ_STATUS_REG      = 0x05,
	AT25DF_CMD_WRITE_STATUS_REG     = 0x01,   // 200 ns max

	AT25DF_CMD_READ_ID              = 0x9F,

	AT25DF_CMD_DEEP_POWER_DOWN      = 0xB9,
	AT25DF_CMD_DEEP_POWER_DOWN_OFF  = 0xAB
};

enum
{
	AT25DF_TIMEOUT_MS_WRITE_PAGE       = 5,
	AT25DF_TIMEOUT_MS_ERASE_CHIP       = 56000,
	AT25DF_TIMEOUT_MS_ERASE_BLOCK4K    = 200,
	AT25DF_TIMEOUT_MS_ERASE_BLOCK32K   = 600,
	AT25DF_TIMEOUT_MS_ERASE_BLOCK64K   = 950,

	AT25DF_TIMEOUT_NS_WRITE_STATUS_REG = 200, // 200 ns max
	AT25DF_TIMEOUT_NS_CS_HIGH          = 50   // 50  ns min
};

enum                               // status register bits
{
	AT25DF_STATUS_SPR_LOCK      = (1<<7),
	AT25DF_STATUS_DUMMY         = (1<<6),
	AT25DF_STATUS_ERROR         = (1<<5),
	AT25DF_STATUS_NWP_PIN_STATE = (1<<4),
	AT25DF_STATUS_SW_PROTECTION = (1<<3)|(1<<2),
	AT25DF_STATUS_WR_EN         = (1<<1),
	AT25DF_STATUS_BUSY          = (1<<0)
};

//=============================================================================
extern uint_fast8_t at25df_busy;


int           at25df_init           (void); // Power-up Device Delay Before Program or Erase Allowed is 10 ms.
uint32_t      at25df_id             (void);

void          at25df_read           (uint32_t address, void* dst,       size_t qty);
int           at25df_write          (uint32_t address, const void* src, size_t qty);
int           at25df_erase          (void);
int           at25df_erase_block4k  (uint32_t address);  // absolute memory address within block
int           at25df_erase_block32k (uint32_t address);  // absolute memory address within block
int           at25df_erase_block64k (uint32_t address);  // absolute memory address within block

uint_fast8_t  at25df_get_swp_bits   (void);              // returns swp bits mask
void          at25df_set_swp_bits   (uint_fast8_t mask);

void          at25df_power_down     (void);
void          at25df_power_down_off (void);

void          at25df_sync           (void); // todo: check???


#endif
