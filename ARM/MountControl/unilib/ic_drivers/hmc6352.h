/*
 * Honeywell HMC6352 I2C 2-axis compass with algorithms   rev.0.01 14.09.2011
 *
 * I2C clk is 100kHz
 *
 * All commands are accepted in the standby mode.
 * Honeywell strongly recommends using this mode during the in stage.
 * Setting up of the HMC6352 operation mode and its slave address are typical set up examples.
 * Although of all commands in the Query and Continuous Modes is acceptable, the completion outcome is not guaranteed!  
 *
 *
 * CMD_GET_DATA:
 * 	In STANDBY MODE: perform one measurement of sensors, compensate andcCalculate new heading. No new measurements are done until another CMD_GET_DATA is sent.
 * 	In QUERY MODE:   perform one measurement of sensors, compensate andcCalculate new heading.

 */

#ifndef _HMC6352_H_
#define _HMC6352_H_

//=============================================================================
// conf section:
//=============================================================================
/*
#define	hmc6352_i2c_read()
#define	hmc6352_i2c_read_block(dst,qty)

#define	hmc6352_i2c_write(x)
#define	hmc6352_i2c_write_block(src,qty)
*/

//=============================================================================
// read-only section
//=============================================================================

enum
{
	HMC6352_WR_ADDRESS = 0x42,
	HMC6352_RD_ADDRESS = 0x43
};

enum
{                                            // code   time to make operation:
	HMC6352_CMD_WR_EEPROM            = 'w',  // 0x77   70  us
	HMC6352_CMD_RD_EEPROM            = 'r',  // 0x72   70  us

	HMC6352_CMD_WR_RAM               = 'G',  // 0x47   70  us
	HMC6352_CMD_RD_RAM               = 'g',  // 0x67   70  us

	HMC6352_CMD_SLEEP_ON             = 'S',  // 0x53   10  us
	HMC6352_CMD_SLEEP_OFF            = 'W',  // 0x57   100 us

	HMC6352_CMD_UPDATE_BRIDGE_OFFSET = 'O',  // 0x4f   6   ms !

	HMC6352_CMD_USR_CALIBRATION_ON   = 'C',  // 0x43   10  us
	HMC6352_CMD_USR_CALIBRATION_OFF  = 'E',  // 0x45   14  ms !

	HMC6352_CMD_SAVE_OPMODE          = 'L',  // 0x4c   125 us

	HMC6352_CMD_GET_DATA             = 'A'   // 0x41   6   ms !
};

enum  // eeprom memory map
{
	HMC6352_EEPROM_MAP_SLAVE_ADDRESS    = 0,  // default: 0x42

	HMC6352_EEPROM_MAP_MAG_X_OFFSET_MSB = 1,
	HMC6352_EEPROM_MAP_MAG_X_OFFSET_LSB = 2,

	HMC6352_EEPROM_MAP_MAG_Y_OFFSET_MSB = 3,
	HMC6352_EEPROM_MAP_MAG_Y_OFFSET_LSB = 4,

	HMC6352_EEPROM_MAP_TIME_DELAY       = 5,  // 0-255 (default:1 - no delay)  0-254 ms

	HMC6352_EEPROM_MAP_SMP_SUMM_QTY     = 6,  // 1-16  (default:4) up to 16 sets of magnetometer data may be selected for averaging

	HMC6352_EEPROM_MAP_SW_REVISION      = 7,  // >1

	HMC6352_EEPROM_MAP_OPERATIONAL_MODE = 8,

	HMC6352_EEPROM_SIZE                 = 9
};

enum  // ram memory map
{
	HMC6352_RAM_MAP_OUTPUT_MODE      = 0x4e, // there is no shadow in eeprom
	HMC6352_RAM_MAP_OPERATIONAL_MODE = 0x74,

	HMC6352_RAM_SIZE                 = 0x75
};

enum
{
	HMC6352_OUTPUT_MODE_HEADING            = 0,  // in tenths of degrees from zero to 3599 and provided in binary format over the two bytes
	HMC6352_OUTPUT_MODE_RAW_X_MAGNETOMETER = 1,  // 10-bit 2's complement binary ADC
	HMC6352_OUTPUT_MODE_RAW_Y_MAGNETOMETER = 2,  //
	HMC6352_OUTPUT_MODE_X_MAGNETOMETER     = 3,  // are the raw magnetometer readings plus offset and scaling factors applied
	HMC6352_OUTPUT_MODE_Y_MAGNETOMETER     = 4   //
};

enum
{
	HMC6352_OPERATIONAL_MODE_STANDBY        = (0<<0), // bits[1,0]
	HMC6352_OPERATIONAL_MODE_QUERY          = (1<<0),
	HMC6352_OPERATIONAL_MODE_CONTINUOUS     = (2<<0),

	HMC6352_OPERATIONAL_MODE_PERIODIC_RESET = (1<<4), // bits[4]

	HMC6352_OPERATIONAL_MODE_RATE_1HZ       = (0<<5), // bits[6,5]
	HMC6352_OPERATIONAL_MODE_RATE_5HZ       = (1<<5),
	HMC6352_OPERATIONAL_MODE_RATE_10HZ      = (2<<5),
	HMC6352_OPERATIONAL_MODE_RATE_20HZ      = (3<<5),

	HMC6352_OPERATIONAL_MODE_MASK           = (3<<0), 
	HMC6352_OPERATIONAL_MODE_RATE_MASK      = (3<<5)
};


//=============================================================================
uint8_t hmc6352_read_output_mode       (void);
int     hmc6352_write_output_mode      (uint8_t mode);
uint8_t hmc6352_read_operational_mode  (void);
int     hmc6352_write_operational_mode (uint8_t mode);
int     hmc6352_init                   (void);
int     hmc6352_calibrate              (int start); // 1-start, 0-stop
int     hmc6352_read                   (void);      // чтение компаса	   (312us)	  все, 70us по частям  (всего 3 части)



#endif
