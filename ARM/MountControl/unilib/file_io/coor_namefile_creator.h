#ifndef _CREATE_NAME_OF_SEARCHED_FILE_H_
#define _CREATE_NAME_OF_SEARCHED_FILE_H_


/*
*
*	need:
*		XCHAR str
*		if ASTGTM2 add prefix add postfix
*		middle part the same for both: the astgtm2_*_dem.tif and *.hgt
*		Middle Part contains:
*			part_lat - 1 symbol: N or S - it's mean nord or south hemispher, 2 and 3 symbols: lat in grad
*			part_lon - 1 symbol: W or E - it's mean west or east hemispher, 1, 2 and 3 symbols: lon in grad
*			extension:
*				if astgtm2 - *.tiff
*				else	   - *.hgt
*
*/

typedef struct
{
    size_t length;
    XCHAR str[30];
    int number_digit;
    int size_extension;
    int size_prefix;
    int offset;
}name_t;

void coord_filename_creator_init(name_t *foldername, name_t *filename);
void coord_filename_creator_task(name_t *foldername, name_t *filename, pos_ll_t *pos);
#ifdef MAIN
	name_t filename;
	name_t foldername;
	uint8_t new_filename_flag;
#else
	extern name_t filename;
	extern name_t foldername;
	extern uint8_t new_filename_flag;
#endif


#endif
