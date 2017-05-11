/*
 *  dprintf.h rev.0.1.2 2012-18-00  [initial: 2010-07-02]
 *
 *  Debug console. 
 *
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 */

#ifndef _DPRINTF_H_
#define _DPRINTF_H_

#include <stdio.h>
#include <stdarg.h>

//=============================================================================
# ifdef NDEBUG
#  define dprintf_init(...)        {;}
#  define dprintf(fmt, ...)        {;}
#  define dprint_array(src,len)    {;}
#  define dprint_binary(src,len)   {;}
#  define dclrscr()                {;}
//=============================================================================
# else
void dprintf_init (void);
void dprintf      (const char* fmt, ...); // don't use it (use dprintf macro instead)
void dprint_array (const void* src, size_t len);
void dprint_binary(const void* src, size_t len); // puts raw binary data w/o any formattings
void dclrscr      (void);
# endif


//=============================================================================
#define dprintx(X)         dprintf("%-17s= 0x%08lx\n", #X, (uint32_t)(X)) // hex (unsigned)
#define dprint(X)          dprintf("%-17s= %ld\n",     #X,  (int32_t)(X)) // dec (signed)
#define dprint_float(X)    dprintf("%-17s= %f\n",      #X,    (float)(X)) // float


#endif
