#include "include.h"


static uint32_t search_digit(char* str);
static uint32_t expansion_my(const char* str);
//static void file_max_number_search(WCHAR *str);

char lfn_buff[255];

//=============================================================================
static uint32_t filename_max_number_search(void *str,  number_t* number)
{	
	char str_digit[5];	
	
	char* ptr = str;
	
	if(expansion_my(ptr))
	{		
		number->length = search_digit(ptr);
		
		for(int i = 0; i < number->length; i++)
		{
			str_digit[i] = *ptr++;
		}
		str_digit[number->length] = '\0';
		
		number->current = atoi(&str_digit[0]);
		
		if(number->current >= 9999)
		{
			return LIMIT_FILE_ERROR;
		}
	}	
	else{return NOT_NEEDED_EXTANSION;}
	
	if(number->current > number->max)
	{									
		number->max = number->current;
	}
	return 0;
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
	name->all_length = strlen(&name->str[0]);
	name->digit_qty  = name->all_length;
	return 1;
}


/*
* Not recommended to union in own catalogue long filename and short filename.
* Example:
*		1234_2015_Mar_25_10:54:00.bin
*		1235.bin
*	In this case if you set the flag_long_name the system find last long name: 1234_2015_Mar_25_10:54:00.bin
* 	and function name_up add 1 to 1234, after this action the system want open this file, but is already exist,
*	because this file are short name 1235.bin
*/
//=============================================================================
uint32_t filename_create_numeric_part(DIR *dj, const XCHAR * name_dir, FILINFO *fno, uint32_t flag_long_name)
{	
	#define DUMMY_DIRECTORY		2
	uint32_t first = 0;
	res_glob = f_opendir(dj, name_dir);
	uint8_t error;
	for(;;)
	{ 
		res_glob = f_readdir(dj, fno);
		if ((fno->fattrib & AM_DIR) != AM_DIR)
        {                  									
			if(flag_long_name)
			{
				if(lfn_convert_16_to_8(&dj->lfn[0]))
				{				
					error = filename_max_number_search(&lfn_buff[0], &number);					
				}				
			}
			else //its for short name <8>-name.<3>-extansion
			{								
				if(fno->fname[0] == 0x00)//its very bad step, but its work. Need search rigth step to definition, what directory is empty
				{
					first++;
				}
				if(first == DUMMY_DIRECTORY){break;}
				if(!first)
				{
					error = filename_max_number_search(&fno->fname[0], &number);
				}
			}					
			if(error)	return error;			
		}
		if(!dj->sect) break; 
	}	
	
	return name_up(&number, &name);
}


//=============================================================================
uint32_t filename_create_utc_time_part(utc_time_t *utc_time, name_t *name)
{	
	name->str[name->all_length] = '_'; //del line terminator
			
	char *p = &utc_time->str[0];
	
	name->all_length++;
	
	memcpy(&name->str[name->all_length], &p[utc_time->YEAR_SERVICE.POS], utc_time->YEAR_SERVICE.LENGTH);	
	
	name->all_length += utc_time->YEAR_SERVICE.LENGTH;
	
	memcpy(&name->str[name->all_length], &p[utc_time->MM_DD_HH_MM_SS_SERVICE.POS], utc_time->MM_DD_HH_MM_SS_SERVICE.LENGTH);
		
	name->all_length += utc_time->MM_DD_HH_MM_SS_SERVICE.LENGTH;
	
	return 1;
}

//=============================================================================
uint32_t filename_create_expansion_part(name_t *name)
{			
	memcpy(&name->str[name->all_length], &name->expansion.str[0], name->expansion.size);
		
	return 1;
}

//=============================================================================
void filename_replace_bad_symbol(char *name)//to do: add another exception
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

