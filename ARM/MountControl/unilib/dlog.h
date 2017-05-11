/*
 *  dlog.h rev.0.1.1 2012-18-01 [initial: 2012-18-01]
 *
 *  Binary debug output logging.
 *
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 */

#ifndef _DLOG_H_
#define _DLOG_H_

#include <stdio.h>
#include <stdarg.h>
#include "dprintf.h"

//=============================================================================
# ifdef NDEBUG
#  define dlog_start(qty)
#  define dlog_append(src, qty)
//=============================================================================
# else
void dlog_start(void);
void dlog_append(const void* src, size_t qty);
# endif


#define dlog(x) dlog_append(&(x),sizeof(x))

void log_photo_task(pic_info_full_t *pic_info, uart_t *uart);
void log_init(void);
void log_error_task(uart_t *uart);
#endif
