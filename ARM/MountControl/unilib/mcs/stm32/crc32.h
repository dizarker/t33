#ifndef _CRC32_H_
#define _CRC32_H_

void crc32hw_init		(void);
uint32_t crc32hw		(const void *buf, int len, int clear);
uint32_t crc32hw_append	(uint32_t crc, const void *buf, uint32_t len);

#endif
