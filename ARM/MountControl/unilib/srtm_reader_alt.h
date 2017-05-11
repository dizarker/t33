#ifndef _SRTM_READER_ALT_H_
#define _SRTM_READER_ALT_H_

#define SRTM_ALT_ERROR 		-32768

int16_t srtm_reader_alt(FIL* fl, const XCHAR *path, pos_ll_t pos, uint8_t* need_open_file);




#endif
