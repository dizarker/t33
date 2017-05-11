#include "include.h"
#ifndef NDEBUG
#include "dprintf_conf.h"

//=============================================================================
int fputc(int c, FILE* stream)
{
	if ((char)c=='\n')
	{
		if (!fputc('\r', stream))
		{
			return (0);
		}
	}

	dputchar(c);

	return (c);
}

//=============================================================================
int fgetc(FILE *f)
{
	return (EOF);
}

//=============================================================================
int ferror(FILE *f)
{
	return (EOF);
}

//=============================================================================
void dprintf_init(void)
{
//	DPRINTF_INIT_COM_PORT();

//	debug_stream = fdevopen(dputchar, NULL);
}

//=============================================================================
void dprintf(const char* fmt, ...)
{
	va_list ap; // arg ptr

	va_start(ap, fmt);

	vfprintf(debug_stream, fmt, ap);

	va_end(ap);
}

//=============================================================================
void dprint_array(const void* src, size_t len)
{
	dprintf("array[%d] : ", (int)len);

	const uint8_t* p = src;

	while (len--)
	{
		dprintf("0x%02x ", *p++);
	}

	dprintf("\n");
}

//=============================================================================
void dprint_binary(const void* src, size_t len) // puts raw binary data w/o any formattings
{
	const uint8_t* p = src;

	while (len--)
	{
		dputchar(*p++);
	}

//	dprintf("\n");
}

//=============================================================================
void dclrscr(void)
{
	dprintf("\x1b[2J"); // Escape sequence: erase Display: clears the screen and moves the cursor to the home position (line 0, column 0)
}

#endif
