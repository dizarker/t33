#include "include.h"

uint_fast8_t at25df_busy;

static uint8_t buf[5]; // cmd + data_word


//=============================================================================
static void at25df_delay_50ns(void)
{
	STATIC_ASSERT(F_CPU<=180000000UL);	 // max 178 MHz is coded in next lines

	(F_CPU>=140000000UL)? __nop(),__nop(),__nop(),__nop(),__nop(),__nop(),__nop(), __nop() : 	// 140-160Mhz 
	(F_CPU>=120000000UL)? __nop(),__nop(),__nop(),__nop(),__nop(),__nop(),__nop() : 			// 120-140
	(F_CPU>=100000000UL)? __nop(),__nop(),__nop(),__nop(),__nop() :  							// 100-120
	(F_CPU>=80000000UL)? __nop(),__nop(),__nop(),__nop() :  									// 80-100
	(F_CPU>=60000000UL)? __nop(),__nop(),__nop() :          									// 60-80
	(F_CPU>=40000000UL)? __nop(),__nop() :                  									// 40-60
	(F_CPU>=20000000UL)? __nop() : 	                        									// 20-40
	(void)0;                                                									// <20 - dummy
}

//=============================================================================
static void at25df_cs_high_delay(void)   // Chip Select High Time is 50 ns.
{
	STATIC_ASSERT(AT25DF_TIMEOUT_NS_CS_HIGH==50);

	at25df_delay_50ns();
}

//=============================================================================
static void at25df_write_status_delay(void)   // 200 ns delay max
{
	for (int i=0; i<AT25DF_TIMEOUT_NS_WRITE_STATUS_REG/50; i++)
	{				
		at25df_delay_50ns();
	}
}

//=============================================================================
static void at25df_on(void)
{
	at25df_spi_on();
	at25df_spi_cs_on();
}

//=============================================================================
static void at25df_off(void)
{
	at25df_spi_cs_off();
	at25df_spi_off();
	at25df_cs_high_delay(); // Chip Select High Time is 50 ns.
}

//=============================================================================
static void at25df_cmd(uint_fast8_t cmd)   // send one byte cmd
{
	at25df_on();

	spi_transfer((uint8_t)cmd);

	at25df_off();
}

//=============================================================================
static void at25df_write_enable(void) // must be exicuted before any writes operation
{
	at25df_cmd(AT25DF_CMD_WRITE_ENABLE);
}

//=============================================================================
static void at25df_write_disable(void)
{
	at25df_cmd(AT25DF_CMD_WRITE_DISABLE);
}

//=============================================================================
void at25df_sync(void)
{
	at25df_write_disable(); // store buffer to flash
}

//=============================================================================
static void at25df_spi_transfer_block(uint8_t* buf, size_t qty)
{
	at25df_on();

	spi_transfer_block(buf, qty);

	at25df_off();
}

//=============================================================================
static void at25df_spi_write_block(const uint8_t* buf, size_t qty)
{
	at25df_on();

	spi_write_block(buf, qty);

	at25df_off();
}

//=============================================================================
static uint_fast8_t at25df_status(void)
{
	buf[0] = AT25DF_CMD_READ_STATUS_REG;

	at25df_spi_transfer_block(buf, 2);

	return (buf[1]);
}

//=============================================================================
static void at25df_write_status(uint_fast8_t status)   // 200 ns max
{
	buf[0] = AT25DF_CMD_WRITE_STATUS_REG;
	buf[1] = (uint8_t)status;

	at25df_write_enable();  // must be exicuted before any writes operation

	at25df_spi_transfer_block(buf, 2);

	at25df_write_status_delay();   // 200 ns delay max
}

//=============================================================================
static uint_fast8_t at25df_ready(void)
{
	return (!(at25df_status() & (uint_fast8_t)AT25DF_STATUS_BUSY));
}

//=============================================================================
// The device also incorporates an intelligent programming algorithm that can detect when a byte location fails to program properly.
// If a programming error arises, it will be indicated by the EPE bit in the Status Register.
//=============================================================================

static uint_fast8_t at25df_error(void)
{
	return ((at25df_status() & (uint_fast8_t)AT25DF_STATUS_ERROR) != 0);
}


//=============================================================================
static uint_fast8_t at25df_wait_us_ready(uint32_t us)
{
	uint_fast8_t ready = 0;

	at25df_busy = 1;

	while (!(ready|=at25df_ready()) && us--)
	{
		delay_us(1);

// fixme
//		main_task();
	}

	at25df_busy = 0;
	
	return (ready);
}

//=============================================================================
void at25df_power_down(void)
{
	at25df_cmd(AT25DF_CMD_DEEP_POWER_DOWN);
} 

//=============================================================================
void at25df_power_down_off(void)
{
	at25df_cmd(AT25DF_CMD_DEEP_POWER_DOWN_OFF);

	delay_us(3); // RDPD time
} 

//=============================================================================
uint32_t at25df_id(void)
{
	buf[0] = AT25DF_CMD_READ_ID;

	at25df_spi_transfer_block(buf, 5);

	return (   ((uint32_t)buf[1]<<24)
	         | ((uint32_t)buf[2]<<16)
	         | ((uint32_t)buf[3]<< 8)
	         | ((uint32_t)buf[4]<< 0));
} 

//=============================================================================
int at25df_init(void) // Power-up device delay before program or erase allowed is 10 ms .
{
	at25df_set_swp_bits(0); // clear sw protection

	return
	(
	   (at25df_status()&AT25DF_STATUS_NWP_PIN_STATE) // and WP# pin is hi (deasserted)
	);
}

//=============================================================================
void at25df_read(uint32_t address, void* dst, size_t qty)
{
	if (dst && qty)
	{
		buf[0] = AT25DF_CMD_READ_ARRAY;
		buf[1] = (uint8_t)(address>>16);
		buf[2] = (uint8_t)(address>>8);
		buf[3] = (uint8_t)(address>>0);
		buf[4] = (uint8_t)(0);           // don't care

		at25df_on();

		spi_write_block(buf, 5);

		spi_transfer_block(dst, qty);

		at25df_off();
	}
}

//=============================================================================
static int at25df_write_block(uint32_t address, const void* src, size_t qty) // unsafe  (w/o page boundary checking)
{
	if (src && qty)
	{
		buf[0] = AT25DF_CMD_WRITE_ARRAY;
		buf[1] = (uint8_t)(address>>16);
		buf[2] = (uint8_t)(address>>8);
		buf[3] = (uint8_t)(address>>0);

		at25df_write_enable();  // must be exicuted before any writes operation

		at25df_on();

		spi_write_block(buf, 4);

		spi_write_block(src, qty);

		at25df_off();
	}

	return (at25df_wait_us_ready(1000UL*AT25DF_TIMEOUT_MS_WRITE_PAGE) && !at25df_error());
}

//=============================================================================
int at25df_write(uint32_t address, const void* src, size_t qty)
{
//	int_fast8_t retry = 0; todo

	while (qty)
	{
		size_t once = AT25DF_PAGE_SIZE - (address % AT25DF_PAGE_SIZE);   // end page align

		if (once>qty)
		{
			once = qty;
		}

		if (!at25df_write_block(address, src, once))
		{
			return (0);
		}

		address += once;  // todo: AT25DF_SIZE
		src      = (void*)((int)src+once);
		qty     -= once;
	}

	return (1);
}


/*
//=============================================================================
int at25df_is_free_area(uint32_t address, size_t len)
{
	while (len--)
	{
		uint_fast8_t x;

		at25df_read(address++, &x, 1);

		if (x!=0xff)
		{
			return (0);
		}		
	}
	
	return (1);
}
*/

//=============================================================================
static void at25df_erase_block(uint32_t address, uint_fast8_t erase_cmd)  // an address within the 4K-, 32K-, or 64K-byte block to be erased must be clocked in
{
	buf[0] = (uint8_t)erase_cmd;
	buf[1] = (uint8_t)(address>>16);
	buf[2] = (uint8_t)(address>>8);
	buf[3] = (uint8_t)(address>>0);

	at25df_write_enable();  // must be exicuted before any writes operation

	at25df_spi_write_block(buf, 4);
}

//=============================================================================
int at25df_erase_block4k(uint32_t address) // absolute memory address within block
{
//	dprintf("at25df_erase_block4k\n");

	at25df_erase_block(address, AT25DF_CMD_ERASE_BLOCK4K);

	return (at25df_wait_us_ready(1000UL*AT25DF_TIMEOUT_MS_ERASE_BLOCK4K) && !at25df_error());
}

//=============================================================================
int at25df_erase_block32k(uint32_t address) // absolute memory address within block
{
	at25df_erase_block(address, AT25DF_CMD_ERASE_BLOCK32K);

	return (at25df_wait_us_ready(1000UL*AT25DF_TIMEOUT_MS_ERASE_BLOCK32K) && !at25df_error());
}

//=============================================================================
int at25df_erase_block64k(uint32_t address) // absolute memory address within block
{
	at25df_erase_block(address, AT25DF_CMD_ERASE_BLOCK64K);

	return (at25df_wait_us_ready(1000UL*AT25DF_TIMEOUT_MS_ERASE_BLOCK64K) && !at25df_error());
}

//=============================================================================
int at25df_erase(void)
{
	at25df_write_enable();  // must be exicuted before any writes operation

	at25df_cmd(AT25DF_CMD_ERASE_CHIP);

	return (at25df_wait_us_ready(1000UL*AT25DF_TIMEOUT_MS_ERASE_CHIP) && !at25df_error());
}

//=============================================================================
uint_fast8_t at25df_get_swp_bits(void)
{
	return (at25df_status() & (uint_fast8_t)AT25DF_STATUS_SW_PROTECTION);
}

//=============================================================================
void at25df_set_swp_bits(uint_fast8_t mask)
{
	uint_fast8_t status_wo_swp = at25df_status() & (uint_fast8_t)~AT25DF_STATUS_SW_PROTECTION;

	at25df_write_status(status_wo_swp & (mask & AT25DF_STATUS_SW_PROTECTION));
}
