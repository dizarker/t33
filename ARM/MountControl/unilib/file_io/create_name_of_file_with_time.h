#ifndef _CREATE_NAME_OF_FILE_WITH_TIME_H_
#define _CREATE_NAME_OF_FILE_WITH_TIME_H_

typedef struct
{
	XCHAR str[15];
	int size;
}expansion_t;

typedef struct
{
	XCHAR str[15];	
}directory_t;


typedef struct
{
    size_t length;
    XCHAR str[60];
    int number_digit;    
    int size_prefix;
    int offset;
	uint32_t all_length;
	
	expansion_t expansion;
	directory_t directory;	
}name_t;

typedef struct
{
	int new;	
	int old;
	uint32_t max;
	uint32_t current;
	uint32_t length;
}number_t;


uint32_t flnLong_create_numeric_part(DIR *dj, const XCHAR * name_dir, FILINFO *fno);
uint32_t flnShort_create_numeric_part(DIR *dj, const XCHAR * name_dir, FILINFO *fno);
uint32_t filename_create_utc_time_part(utc_time_t *utc_time, name_t *name);
void filename_replace_bad_symbol(char *name);
uint32_t filename_create_expansion_part(name_t *name);

#ifdef MAIN
name_t name;
number_t number;

const name_t name_const  = 
	{
		0,
		"0000000000000000000000000000000.bin\0",
		0,
		0,
		0,
		0,
		{
			".bin\0",
			5
		},
		{
			"/AFS_LOG\0",	
		}
	};
const number_t number_const  = 
	{
		0,
		0,
		0,
		0,
		0
	};
	
#else
extern name_t name;
extern number_t number;
	
extern const name_t name_const;
extern const number_t number_const;
#endif

#endif
