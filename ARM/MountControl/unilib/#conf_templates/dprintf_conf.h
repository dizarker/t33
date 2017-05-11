#ifndef _DPRINTF_CONF_H_
#define _DPRINTF_CONF_H_

struct __FILE { int handle;};
FILE __stdout;
FILE __stdin;
#define debug_stream   &__stdout

#ifndef ITM_DEBUG
#	define dputchar(x)  putchar2((x))
#else
#	define dputchar(x)  ITM_SendChar((x)) // inside core_cm3.h (cmsis)
#endif

#endif
