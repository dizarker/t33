#include "include.h"


//=============================================================================
static int hmc6352_i2c_write_block(uint8_t* buf, size_t qty)
{
	i2c_start();

	int res = i2c_write_block(buf, qty); // writes bytes to bus w/o start/stop conditions 

	i2c_stop();

	return (res);
}

//=============================================================================
static uint16_t hmc6352_read_buffer(size_t qty)  // read 1..2 bytes of output regs
{
	uint16_t data = 0;

	if (qty && (qty<=2))
	{
		i2c_start();

		if (i2c_write_byte(HMC6352_RD_ADDRESS))
		{
			uint8_t buf[2] = {0, 0}; // hi=0, lo=0

			i2c_read_block(buf, qty);

			data = ((buf[0]<<((qty-1)*8)) + buf[1]);   // big-engian to target-endian
		}

		i2c_stop();
	}

	return (data);
}

//=============================================================================
// reading ram & eeprom section
//=============================================================================
static uint16_t hmc6352_rd_any(int address, size_t qty, int rd_cmd)   // read 1..2 bytes of ram or eeprom
{
	if (qty && (qty<=2))
	{
		uint8_t buf[3] = {HMC6352_WR_ADDRESS, rd_cmd, address}; // not RD!

		if (hmc6352_i2c_write_block(buf, sizeof(buf)))          // write: address+read_cmd+data_addres
		{

delay_ms(10);

			return (hmc6352_read_buffer(qty));                  // then read 1..2 bytes
		}
	}

	return (0); // todo ¢®§¢à é âì ¤ ­­ë¥ ­¥ äã­ªæ¨¥©   ç¥à¥§ ãª § â¥«ì
}

static uint16_t hmc6352_rd_eeprom     (int ee_address,  size_t qty) {return (hmc6352_rd_any   (ee_address, qty, HMC6352_CMD_RD_EEPROM));}
static uint8_t  hmc6352_rd_eeprom_byte(int ee_address)              {return (hmc6352_rd_eeprom(ee_address, 1));}
//static uint16_t hmc6352_rd_eeprom_word(int ee_address)              {return (hmc6352_rd_eeprom(ee_address, 2));}

static uint16_t hmc6352_rd_ram        (int ram_address, size_t qty) {return (hmc6352_rd_any   (ram_address, qty, HMC6352_CMD_RD_RAM  ));}
static uint8_t  hmc6352_rd_ram_byte   (int ram_address)             {return (hmc6352_rd_ram   (ram_address, 1));}
//static uint16_t hmc6352_rd_ram_word   (int ram_address)             {return (hmc6352_rd_ram   (ram_address, 2));}

//=============================================================================
// writing ram & eeprom section
//=============================================================================
static int hmc6352_wr_any(int address, uint16_t data, size_t qty, int wr_cmd)   // writes 1..2 bytes of ram or eeprom
{
	if (qty && (qty<=2))
	{
		data <<= 8*(2-qty); // if qty=1 send only low byte (move low to hi position)

		uint8_t buf[3+2] = {HMC6352_WR_ADDRESS, wr_cmd, address, BYTE1(data), BYTE0(data)};

		return (hmc6352_i2c_write_block(buf, 3+qty));
	}

	return (0);
}

static int hmc6352_wr_eeprom     (int ee_address,  uint16_t data, size_t qty)  {return (hmc6352_wr_any   (ee_address,  data, qty, HMC6352_CMD_WR_EEPROM));}
static int hmc6352_wr_eeprom_byte(int ee_address,  uint8_t  data)              {return (hmc6352_wr_eeprom(ee_address,  data, 1));}
//static int hmc6352_wr_eeprom_word(int ee_address,  uint16_t data)              {return (hmc6352_wr_eeprom(ee_address,  data, 2));}

static int hmc6352_wr_ram        (int ram_address, uint16_t data, size_t qty)  {return (hmc6352_wr_any   (ram_address, data, qty, HMC6352_CMD_WR_RAM   ));}
static int hmc6352_wr_ram_byte   (int ram_address, uint8_t  data)              {return (hmc6352_wr_ram   (ram_address, data, 1));}
//static int hmc6352_wr_ram_word   (int ram_address, uint16_t data)              {return (hmc6352_wr_ram   (ram_address, data, 2));}


//=============================================================================
// common section
//=============================================================================

uint8_t hmc6352_read_slave_address(void)             {return (hmc6352_rd_eeprom_byte(HMC6352_EEPROM_MAP_SLAVE_ADDRESS));}
int     hmc6352_write_slave_address(uint8_t address) {return (hmc6352_wr_eeprom_byte(HMC6352_EEPROM_MAP_SLAVE_ADDRESS, address));}

//=============================================================================
uint8_t hmc6352_read_output_mode  (void)             {return (hmc6352_rd_ram_byte(HMC6352_RAM_MAP_OUTPUT_MODE));}
int     hmc6352_write_output_mode (uint8_t mode)     {return (hmc6352_wr_ram_byte(HMC6352_RAM_MAP_OUTPUT_MODE, mode));} // there is no shadow in eeprom

//=============================================================================
uint8_t hmc6352_read_operational_mode(void)          {return (hmc6352_rd_eeprom_byte(HMC6352_EEPROM_MAP_OPERATIONAL_MODE));}
int     hmc6352_write_operational_mode(uint8_t mode) {return (hmc6352_wr_eeprom_byte(HMC6352_EEPROM_MAP_OPERATIONAL_MODE, mode));}


//=============================================================================
int hmc6352_init(void)
{
	delay_ms(300); // it is important !

	hmc6352_write_operational_mode(HMC6352_OPERATIONAL_MODE_STANDBY);  // all cfg in standby mode


	hmc6352_write_output_mode(HMC6352_OUTPUT_MODE_HEADING);
	hmc6352_write_operational_mode(HMC6352_OPERATIONAL_MODE_QUERY);


dprintx(hmc6352_read_output_mode());

dprintx(hmc6352_read_slave_address());

//dprintx(hmc6352_read_operational_mode());


/*
	if (hmc6352_read_slave_address()!=HMC6352_WR_ADDRESS)
	{
		dprintf("hmc6352: bad slave address!\n");
		return (0);
	}

	if (!hmc6352_write_output_mode(HMC6352_OUTPUT_MODE_HEADING))
	{
		dprintf("hmc6352: fail set output_mode!\n");
		return (0);
	}

	if (!hmc6352_write_operational_mode(HMC6352_OPERATIONAL_MODE_QUERY)) // todo: periodic set/reset=1
	{
		dprintf("hmc6352: fail set operational_mode!\n");
		return (0);
	}
*/
	return (1);
}

//=============================================================================
int hmc6352_calibrate(int start) // 1-start, 0-stop
{
	uint8_t cmd    =  (start) ?  HMC6352_CMD_USR_CALIBRATION_ON : HMC6352_CMD_USR_CALIBRATION_OFF;

	uint8_t buf[2] = {HMC6352_WR_ADDRESS, cmd};

	return (hmc6352_i2c_write_block(buf, sizeof(buf)));
}

//=============================================================================
int hmc6352_read(void) // ÷òåíèå êîìïàñà	   (312us)	  âñå, 70us ïî ÷àñòÿì  (âñåãî 3 ÷àñòè)
{
	return (hmc6352_read_buffer(2));  // read 2 bytes of output regs

//	return (3600-(a1*256+b1));
}
