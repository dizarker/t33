#ifndef _CRC32_SOFTWARE_H_
#define _CRC32_SOFTWARE_H_

void crc32sftwr_init(void);
uint32_t crc32_sftwr(uint32_t init_crc, uint8_t *buf, int len);
#endif
