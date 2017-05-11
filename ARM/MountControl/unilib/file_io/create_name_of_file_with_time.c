#include "include.h"


static uint32_t search_digit(char* str);
static uint32_t expansion_my(const char* str);
//static void file_max_number_search(WCHAR *str);

char lfn_buff[255];

//=============================================================================
static uint32_t filename_max_number_search(char *str,  number_t* number)
{	
	char str_digit[5];	
		
	if(expansion_my(str))
	{		
		number->length = search_digit(str);
		
		for(int i = 0; i < number->length; i++)
		{
			str_digit[i] = *str++;
		}
		str_digit[number->length] = '\0';
		
		number->current = atoi(&str_digit[0]);
		
		if(number->current >= 9999)
		{
			return 0;
		}
	}
	
	if(number->current > number->max)
	{									
		number->max = number->current;
	}
	return 1;
}

//=============================================================================
static uint32_t lfn_convert_16_to_8(uint16_t* str16)
{
	for(int i = 0; i < sizeof(lfn_buff); i++)
	{				
		uint16_t help = *str16++;
		lfn_buff[i] = help;
	}
	return 1;
}

//=============================================================================
static uint32_t name_up(number_t* number, name_t* name)
{		
	number->max++;
	sprintf(&name->str[0], "%d", number->max);
	name->number_digit = strlen(&name->str[0]);
	return 1;
}



//=============================================================================
uint32_t flnLong_create_numeric_part(DIR *dj, const XCHAR * name_dir, FILINFO *fno)
{
	#define LIMIT_FILE_ERROR	0
	res_glob = f_opendir(dj, name_dir);	
	for(;;)
	{ 
		res_glob = f_readdir(dj, fno);
		if ((fno->fattrib & AM_DIR) != AM_DIR)
        {                  									
			if(lfn_convert_16_to_8(&dj->lfn[0]))
			{
				if(!filename_max_number_search(&lfn_buff[0], &number))
				{
					return LIMIT_FILE_ERROR;
				}
			}
		}
		if(!dj->sect) break; 
	}	
	
	return name_up(&number, &name);
}


//=============================================================================
uint32_t filename_create_utc_time_part(utc_time_t *utc_time, name_t *name)
{	
	name->str[name->number_digit] = '_';
	
	//memcpy(&name->str[name->number_digit + 1], utc_time->str[0], 24);
	
	char *p = &utc_time->str[0];
	
	name->number_digit++;	
	memcpy(&name->str[name->number_digit], &p[20], 4);	
	
	name->number_digit += 4;
	memcpy(&name->str[name->number_digit], &p[3], 16);
	
	name->number_digit += 16;
	
	name->all_length += name->number_digit;
	return 1;
}

//=============================================================================
uint32_t filename_create_expansion_part(name_t *name)
{		
	memcpy(&name->str[name->all_length], &name->expansion.str[0], name->expansion.size);
		
	return 1;
}

//=============================================================================
void filename_replace_bad_symbol(char *name)
{
	uint32_t size = strlen(&name[0]);
	for(int i = 0; i < size; i++)
	{
		if(*name == ' '){*name = '_';}
		if(*name == ':'){*name = '-';}
		*name++;
	}
}

//=============================================================================
static uint32_t search_digit(char* str)
{
	int i = 0;
	int digit_x = 0;
	while(str[i] != '\0')
	{
		if(str[i] >= '0' && str[i] <= '9')
		{
			digit_x++;
		}
		if(str[i] == '.' || str[i] == '_')
		{
			return digit_x;
		}
		i++;
	}
	return NULL;
}

//=============================================================================
static uint32_t expansion_my(const char* str)
{
    uint32_t state = 0;
    uint32_t start_search = 0;
    while(*str != '\0')
    {
        if(*str == '.'){start_search = 1;}
        if(start_search)
        {
            if(state == 0)
            {
                if(*str == 'b')
                {
                        state = 1;
                        *str++;
                        continue;
                }
                else{state = 0; *str++;}
            }
            if(state == 1)
            {
                if(*str == 'i')
                {
                        state = 2;
                        *str++;
                        continue;
                }
                else{state = 0; *str++;}
            }
            if(state == 2)
            {
                if(*str == 'n')
                {
                        state = 0;
                        *str++;
                        return 1; //exist
                }
                else{state = 0; *str++;}
            }
        }
        else
        {
            *str++;
        }
    }
    return 0;
};

/*
	This function use if name of file is <8>.<3>, where - <is unique name of file>.<extansion>
*/
//=============================================================================
uint32_t flnShort_create_numeric_part(DIR *dj, const XCHAR * name_dir, FILINFO *fno)
{
	/*#define DUMMY_DIRECTORY		2
	
	res_glob = f_opendir(dj, name_dir);
	for(;;)
	{ 
		res_glob = f_readdir(dj, fno);
		if ((fno->fattrib & AM_DIR) != AM_DIR)
        {                  			
			if(fno->fname[0] == 0x00)
			{
				first++;			
			}
			if(first == DUMMY_DIRECTORY){break;}
			if(!first)
			{
				//file_max_number_search(&fno->fname[0]);
			}
		 }
		 else if(fno->fname[0] == 0x00){break;}
	}
	first = 0;
	name_up();	
	*/
	return 0;
}
