#include "include.h"

/*
*
*	https://wiki.orlan.in/doku.php/%D0%B1%D1%80%D1%8D%D0%BE:unilib:srtm_alt_reader#%D1%81%D1%81%D1%8B%D0%BB%D0%BA%D0%B0_%D0%BD%D0%B0_%D0%B4%D0%BE%D0%BA%D1%83%D0%BC%D0%B5%D0%BD%D1%82%D0%B0%D1%86%D0%B8%D1%8E
*
*
*
*
*
*/


#define COLOUMN_MATRIX_SIZE	1201
#define ROW_MATRIX_SIZE		1201

//=============================================================================
int16_t srtm_reader_alt(FIL* fl, const XCHAR *path, pos_ll_t pos, uint8_t* need_open_file)
{	
	static uint8_t res = 0;
	if(*need_open_file)
	{
		res = f_open(fl, path, FA_READ);
		*need_open_file = 0;
	}
	if(!res)
	{
		//transform fractional part to sec
		uint32_t offset_lat = (pos.lat - (int32_t)pos.lat)*3600;
		uint32_t offset_lon = (pos.lon - (int32_t)pos.lon)*3600;
				
		uint32_t row = (offset_lat)/3;
		uint32_t coloumn = (offset_lon)/3;
		
		uint32_t offset_all = ((ROW_MATRIX_SIZE - row) * COLOUMN_MATRIX_SIZE + coloumn) * 2;
		
		if(offset_all > fl->fsize){return ERROR_IN_AP3_PROTO;}
		
		f_lseek(fl, offset_all);
		
		int16_t sense_alt = 0;
		uint32_t qty_write = 0;
		
		f_read(fl, &sense_alt, 2, &qty_write);
		
		sense_alt = read_word_bendian(&sense_alt);
		
		if(sense_alt == SRTM_ALT_ERROR)
		{
			return SRTM_ALT_ERROR;
		}			

		return (sense_alt);
	}

	return SRTM_ALT_ERROR;
}


