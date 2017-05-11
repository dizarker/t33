#include "include.h"


#define HGT_VER
//#define ASTGTM2_VER

#define FOLDER_PREFIX_STR         "HeightMaps/"
#define FOLDER_PREFIX_STR_SIZE    (sizeof(FOLDER_PREFIX_STR)-1)

/*
*
*
*	if ASTGTM2 add prefix add postfix
*	middle part the same for both: the astgtm2_*_dem.tif and *.hgt
*			1. ASTGTM2_N08E020_dem.tif
*			2. N58W105.hgt
*	Middle Part containe:
*		part_lat - 1 symbol: N or S - it's mean nord or south hemispher, 2 and 3 symbols: lat in grad
*		part_lon - 1 symbol: W or E - it's mean west or east hemispher, 1, 2 and 3 symbols: lon in grad
*		extension:
*			if astgtm2 - *.tiff
*			else	   - *.hgt
* The structure of folder on SD is:
*		N/
*			E/files
*			W/files
*		S/
*			E/files
*			W/files
*/

static void 	coord_filename_add_prefix	(name_t *foldername, name_t *filename);
static void 	transform_floatCoor_to_str	(XCHAR *src, float lat, float lon);
static void 	filename_replace_bad_symbol	(char *name);
static uint32_t filename_create				(name_t *name, pos_ll_t *pos);
static uint32_t foldername_create			(name_t *name, pos_ll_t *pos);
static XCHAR arr_coor[6];

enum
{
	NE = 1,
	NW = 2,
	SW = 3,
	SE = 4
};

	
//=============================================================================
void coord_filename_creator_init(name_t *foldername, name_t *filename)
{
	coord_filename_add_prefix(foldername, filename);
}

//=============================================================================
void coord_filename_creator_task(name_t *foldername, name_t *filename, pos_ll_t *pos)
{	
	if(!foldername_create(foldername, pos))
	{
		dprintf("Folderename: Hemisphere definition error, wrong coor\n");
		alt_system.sense_metr.ground = ERROR_IN_AP3_PROTO;
	}
	if(!filename_create(filename, pos))
	{	
		dprintf("Filename: Hemisphere definition error, wrong coor\n");
		alt_system.sense_metr.ground = ERROR_IN_AP3_PROTO;
	}
}

//=============================================================================
static void coord_filename_add_prefix(name_t *foldername, name_t *filename)
{
	uint32_t filename_prefix_size 	= 0;
	
	#if defined(ASTGTM2_VER)
	
		filename_prefix_size = 8;
		memcpy(&name->str[0], "ASTGTM2_", offset);
	
	#elif defined(HGT_VER)
		filename_prefix_size = FOLDER_PREFIX_STR_SIZE;
		memcpy(&filename->str[0], FOLDER_PREFIX_STR, FOLDER_PREFIX_STR_SIZE);
	#endif
	
	filename->offset = filename_prefix_size;
	filename->length += filename_prefix_size;
	
	foldername->offset = FOLDER_PREFIX_STR_SIZE;
	
	memcpy(&foldername->str[0], FOLDER_PREFIX_STR, FOLDER_PREFIX_STR_SIZE);	
}

//=============================================================================
static void transform_floatCoor_to_str(XCHAR *src, float lat, float lon)
{		
	sprintf(&src[0], "%2d", ABS((int32_t)lat));
	sprintf(&src[2], "%3d", ABS((int32_t)lon));	
}

//=============================================================================
static void filename_replace_bad_symbol(char *name)//to do: add another exception
{
	uint32_t size = strlen(&name[0]);
	for(int i = 0; i < size; i++)
	{
		if(*name == ' '){*name = '0';}
		if(*name == ':'){*name = '-';}
		*name++;
	}
}

//=============================================================================
static uint32_t foldername_create(name_t *foldername, pos_ll_t *pos)
{	
	static const uint32_t HEMISPHERE_SIZE = 5;
	
	foldername->length = foldername->offset;
	
	switch(coor_get_hemisphere(pos->lat, pos->lon))
	{
		case NE:
			memcpy(&foldername->str[foldername->length], 	"N/E\0", HEMISPHERE_SIZE);						
			break;
		case NW:
			memcpy(&foldername->str[foldername->length], 	"N/W\0", HEMISPHERE_SIZE);			
			break;
		case SW:
			memcpy(&foldername->str[foldername->length], 	"S/W\0", HEMISPHERE_SIZE);			
			break;
		case SE:
			memcpy(&foldername->str[foldername->length], 	"S/E\0", HEMISPHERE_SIZE);			
			break;
		default:
			return 0;
	}
	foldername->length = foldername->offset + HEMISPHERE_SIZE;
	
	dprintf("folder name:%s\n", &foldername->str[0]);
	return 1;
}

//=============================================================================
static uint32_t filename_create(name_t *name, pos_ll_t *pos)
{			
	enum
	{
		HEMISPHERE_PREFIX_STR_SIZE 	= 5,
		HEMISPHERE_STR_SIZE 	= 1,
		LAT_STR_SIZE 			= 2,
		LON_STR_SIZE 			= 3		
	};
	
	transform_floatCoor_to_str(&arr_coor[0], pos->lat, pos->lon);
	
	name->length = name->offset;
	
	switch(coor_get_hemisphere(pos->lat, pos->lon))
	{
		case NE:			
			memcpy(&name->str[name->length], "N/E/N", 5);
			name->length += HEMISPHERE_PREFIX_STR_SIZE;
			memcpy(&name->str[name->length], &arr_coor[0], 2);
			name->length += LAT_STR_SIZE;
			memcpy(&name->str[name->length], "E", 1);				
			break;
		case NW:
			memcpy(&name->str[name->length], "N/W/N", 5);
			name->length += HEMISPHERE_PREFIX_STR_SIZE;
			memcpy(&name->str[name->length], &arr_coor[0], 2);
			name->length += LAT_STR_SIZE;
			memcpy(&name->str[name->length], "W", 1);
			break;
		case SW:
			memcpy(&name->str[name->length], "S/W/S", 5);
			name->length += HEMISPHERE_PREFIX_STR_SIZE;
			memcpy(&name->str[name->length], &arr_coor[0], 2);
			name->length += LAT_STR_SIZE;
			memcpy(&name->str[name->length], "W", 1);
			break;
		case SE:
			memcpy(&name->str[name->length], "S/E/S", 1);
			name->length += HEMISPHERE_PREFIX_STR_SIZE;
			memcpy(&name->str[name->length], &arr_coor[0], 2);
			name->length += LAT_STR_SIZE;
			memcpy(&name->str[name->length], "E", 1);
			break;
		
		default:
			return 0;	
	}
	
	name->length += HEMISPHERE_STR_SIZE;
	
	//filename_replace_bad_symbol(&arr_coor[0]);
		
	memcpy(&name->str[name->length], &arr_coor[2], 3);
	
	name->length += LON_STR_SIZE;
	
	#if defined(ASTGTM2_VER)
		
		#define POSTFIX_ASGTM_SIZE	11
		memcpy(&name->str[name->length], "_dem.tiff\0", POSTFIX_ASGTM_SIZE);	
	
	#elif defined(HGT_VER)
	
		#define POSTFIX_HGT_SIZE 	5
		memcpy(&name->str[name->length], ".hgt\0", POSTFIX_HGT_SIZE);
	
	#endif
	
	filename_replace_bad_symbol(&name->str[0]);
	
	dprintf("file name:%s\n", &name->str[0]);
	
	return 1;
}

