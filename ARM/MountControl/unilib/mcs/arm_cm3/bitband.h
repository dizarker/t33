/*
 *  bitband.h rev.1.0.0 2011-06-28
 *
 *  Cortex-m3 bitband accessor, based on: http://electronix.ru/forum/index.php?showtopic=94004&view=findpost&p=968577
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 *
 * A mapping formula shows how to reference each word in the alias region to a corresponding bit in the bit-band region.
 *
 * The mapping formula is:
 *   bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number * 4)
 * where:
 *   bit_word_addr - is the address of the word in the alias memory region that maps to the targeted bit.
 *   bit_band_base - is the starting address of the alias region
 *   byte_offset   - is the number of the byte in the bit-band region that contains the targeted bit
 *   bit_number    - is the bit position (0-7) of the targeted bit.
 *
 * Example1:
 * The following example shows how to map bit 2 of the byte located at SRAM address
 *   0x20000300 in the alias region:
 *   0x22006008 = 0x22000000 + (0x300*32) + (2*4).
 * Writing to address 0x22006008 has the same effect as a read-modify-write operation on bit 2 of the byte at SRAM address 0x20000300.
 * Reading address 0x22006008 returns the value (0x01 or 0x00) of bit 2 of the byte at SRAM address 0x20000300 (0x01: bit set; 0x00: bit reset).
 * 
 * Example2:
 * BITBAND(GPIOC->ODR, 8) = ~BITBAND(GPIOC->ODR, 8);
 * 
 * Example3:
 * #define MODBUS_FLAG_SET(a) do {*(volatile uint32_t*)(MB_Addr_BB_Coils + a*4) = 1;} while (0)
 * 
 */

#ifndef _BITBAND_H_
#define _BITBAND_H_

/*
#define BB_BIT(REGISTER, BIT_NUMBER, BASE, BB_BASE) \
	((volatile s32*) ((BB_BASE) + (((u32) &(REGISTER)) - (BASE)) * 32 + (BIT_NUMBER) * 4))

#define PERIPHERAL_BIT(REGISTER, BIT_NUMBER) \
	BB_BIT(REGISTER, BIT_NUMBER, PERIPH_BASE, PERIPH_BB_BASE)

#define SRAM_BIT(REGISTER, BIT_NUMBER) \
	BB_BIT(REGISTER, BIT_NUMBER, SRAM_BASE, SRAM_BB_BASE)

*/


#define BITBAND(WPTR, BITX)                                                      \
  *((volatile uint32_t*) (((uint32_t)&(WPTR) & 0xf0000000UL)     | 0x02000000UL  \
                        +(((uint32_t)&(WPTR) & 0x000fffffUL)<<5) | ((BITX)<<2)))

#endif

