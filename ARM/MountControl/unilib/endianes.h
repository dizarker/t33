/* 
 *  endianes.h rev.2.0.0 2011-02-01
 *
 *  Endianes data accessor. 
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 * 
 */

#ifndef _ENDIANES_H_
#define _ENDIANES_H_

//=============================================================================
uint_fast16_t read_word_lendian  (const void* src_lendian);
uint_fast16_t read_word_bendian  (const void* src_bendian);

void          write_word_lendian (void* dst_lendian, uint_fast16_t x);
void          write_word_bendian (void* dst_bendian, uint_fast16_t x);

uint32_t      read_dword_lendian (const void* src_lendian);
uint32_t      read_dword_bendian (const void* src_bendian);

void          write_dword_lendian(void* dst_lendian, uint32_t x);
void          write_dword_bendian(void* dst_bendian, uint32_t x);

float         read_float_lendian (const void* src_lendian);
float         read_float_bendian (const void* src_bendian);

void          write_float_lendian(void* dst_lendian, float x);
void          write_float_bendian(void* dst_bendian, float x);


#endif
