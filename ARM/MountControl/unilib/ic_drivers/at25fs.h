/* 
 * at25 series memory circular file system
 * 
 *
 */

#ifndef _AT25FS_H_
#define _AT25FS_H_

//=============================================================================
// conf section:
//=============================================================================
/*#define AT25FS_SIZE_FOR_REG			4
#define AT25FS_SIZE_OFFSET_REG		AT25DF_SIZE //used third part of memory  - AT25FS_SIZE_FOR_REG*AT25DF_PAGE_SIZE
#define AT25FS_QTY_SEC_FOR_REG		AT25FS_SIZE_FOR_REG*/

/*
*	
*							Size		Offset
*	1. id 	 				4kB			0
*   2. Info type pendant 	4kB			4kB
*   3. Type cell list		8kb			8kB
*	4. Param point			2040kB		16kB
*	5. Out point			2040kB		2056kB
*
*/


#define AT25FS_ID_OFFSET           				0
#define AT25FS_ID_SIZE           				AT25DF_BLOCK_4K_SIZE
#define AT25FS_PENDANT_INFO_OFFSET   			AT25FS_ID_SIZE
#define AT25FS_PENDANT_INFO_SIZE           		AT25DF_BLOCK_4K_SIZE
#define AT25FS_TYPE_CELL_OFFSET   				(AT25FS_PENDANT_INFO_SIZE + AT25FS_ID_SIZE)
#define AT25FS_TYPE_CELL_SIZE           		AT25DF_BLOCK_4K_SIZE
#define AT25FS_POINT_INFO_LIST_CONFIRM_OFFSET	(AT25FS_PENDANT_INFO_SIZE + AT25FS_ID_SIZE + AT25FS_TYPE_CELL_SIZE)
#define AT25FS_POINT_INFO_LIST_CONFIRM_SIZE		AT25DF_BLOCK_4K_SIZE
#define AT25FS_OFFSET_POINT_PARAM               (AT25FS_ID_SIZE + AT25FS_PENDANT_INFO_SIZE + AT25FS_TYPE_CELL_SIZE + AT25FS_POINT_INFO_LIST_CONFIRM_SIZE)
#define AT25FS_POINT_PARAM_SIZE					((AT25DF_SIZE - AT25FS_PENDANT_INFO_SIZE - AT25FS_ID_SIZE - AT25FS_TYPE_CELL_SIZE - AT25FS_POINT_INFO_LIST_CONFIRM_SIZE)/2)

#define AT25FS_SIZE                ((AT25DF_SIZE - AT25FS_PENDANT_INFO_SIZE - AT25FS_ID_SIZE - AT25FS_TYPE_CELL_SIZE - AT25FS_POINT_INFO_LIST_CONFIRM_SIZE)/2)
#define AT25FS_OFFSET              (AT25FS_POINT_PARAM_SIZE + AT25FS_PENDANT_INFO_SIZE + AT25FS_ID_SIZE + AT25FS_TYPE_CELL_SIZE + AT25FS_POINT_INFO_LIST_CONFIRM_SIZE)
#define AT25FS_BLOCK_SIZE          AT25DF_BLOCK_4K_SIZE   // 4K
#define AT25FS_PAGE_SIZE           AT25DF_PAGE_SIZE

#define AT25FS_REC_FULL_SIZE       64U                     
#define AT25FS_REC_DATA_SIZE       (AT25FS_REC_FULL_SIZE-sizeof(at25fs_rec_header_t))

#define AT25FS_MAX_RECS_QTY        (AT25FS_SIZE/AT25FS_REC_FULL_SIZE)

#define AT25FS_INVALID_REC_NUMBER  ((uint32_t)-1)

STATIC_ASSERT(AT25FS_OFFSET    % AT25FS_BLOCK_SIZE    == 0);  // offset is on block boundary
STATIC_ASSERT(AT25FS_PAGE_SIZE % AT25FS_REC_FULL_SIZE == 0);  //


//=============================================================================
// don't modify below:
//=============================================================================

enum
{
	AT25FS_TRUE  = 0xff,
	AT25FS_FALSE = 0x00  // we only can write and erase (erased = 0xff)
};

#pragma pack(push, 1)

typedef struct
{
	struct
	{
		uint8_t  empty;
		uint8_t  valid;
	} status;
} at25fs_rec_header_t;

typedef struct
{
	at25fs_rec_header_t header;
	uint8_t             data[AT25FS_REC_DATA_SIZE];  // data + dummy bytes to 64 bytes size
} at25fs_rec_t;

#pragma pack(pop)

typedef struct
{
	uint32_t     next_rec_number;

} at25fs_t;


//=============================================================================
void     at25fs_init                (void);
int      at25fs_read_rec_data       (uint32_t rec_number, void* data_dst, size_t len);


uint32_t at25fs_get_next_rec_number (void);
int      at25fs_delete_rec          (uint32_t rec_number);
void     at25fs_delete_all_recs     (void);

int      at25fs_append              (const void* data_src, size_t len);



#endif
