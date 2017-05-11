/* 
 * Pak protocol.
 *
 *  Serial port cfg:    115200-8-n-1
 *  Pack prefix:        xB0\x3B\x4F
 *  Fields format:      little-endian
 *  CRC16 (CCIIT):      init:0xffff, poly:?
 *
 */


#ifndef _PAK_PROTO_CORE_H_
#define _PAK_PROTO_CORE_H_

#define PAK_PROTO_UART                   uart_modem

#define PAK_PROTO_REV                     7
#define PAK_PROTO_REV_STR                "7.0"

#define PAK_PROTO_PREFIX_STR             "\xB0\x3B\x4F"
#define PAK_PROTO_PREFIX_SIZE            (sizeof(PAK_PROTO_PREFIX_STR)-1)

#define PAK_PROTO_CRC_SIZE               (sizeof(uint16_t))

#define PAK_PROTO_MAX_FRAME_SIZE         256

#define PAK_PROTO_MAX_DATA_SIZE          (PAK_PROTO_MAX_FRAME_SIZE - (sizeof(pak_proto_frame_header_t) - PAK_PROTO_PREFIX_SIZE) - PAK_PROTO_CRC_SIZE)  //  256-(9-3)-2 = 256-8 = 248

#define PAK_PROTO_HEADER_SIZE            sizeof(pak_proto_frame_header_t)

#define PAK_PROTO_BAD_FRAME_NUMBER       ((uint16_t)-1)

#define PAK_PROTO_ACK_BIT_MASK           (1<<7)     // msb of address

#define PAK_PROTO_BROADCAST_ADDRESS      0  // âñåì ñðàçó è áåç ïîäòâåðæäåíèÿ
#define PAK_PROTO_NPU_ADDRESS            1  // àäðåñ áàçû (íàçåìêà)
#define PAK_PROTO_LIMB_ADDRESS           2  // ìîé àäðåñ  (ïðèáîð íà áîðòó)

enum
{
	PAK_OK = 0,                // 0
	PAK_ERR_DUP_PACK,          // 1 (Ð¿Ð¾Ð»ÑƒÑ‡ÐµÐ½ Ð¿Ð°ÐºÐµÑ‚ c Ð¿Ñ€ÐµÐ´Ñ‹Ð´ÑƒÑ‰Ð¸Ð¼ Ð½Ð¾Ð¼ÐµÑ€Ð¾Ð¼ (Ð¿Ð¾Ð²Ñ‚Ð¾Ñ€Ð½Ð¾) - ÑÑ‚Ð¾ ÑÐ¾Ð±ÑÑ‚Ð²ÐµÐ½Ð½Ð¾ Ð¸ Ð½Ðµ Ð¾ÑˆÐ¸Ð±ÐºÐ°, Ð½Ð¾ Ð¿Ð¾Ð·Ð²Ð¾Ð»Ð¸Ñ‚ Ð¾Ñ†ÐµÐ½Ð¸Ñ‚ÑŒ ÐºÐ°Ñ‡ÐµÑÑ‚Ð²Ð¾ ÑÐ²ÑÐ·Ð¸)
	PAK_ERR_CRC16,             // 2 (Ð¿Ð¾Ð»ÑƒÑ‡ÐµÐ½ Ð¿Ð°ÐºÐµÑ‚ c Ð±Ð¸Ñ‚Ñ‹Ð¼ CRC16)
	PAK_ERR_BAD_DATA_SIZE,     // 3 (Ñ€Ð°Ð·Ð¼ÐµÑ€ Ð¿Ð°ÐºÐµÑ‚Ð° Ð½Ðµ ÐºÐ¾Ñ€ÐµÐ»Ð¸Ñ€ÑƒÐµÑ‚ Ñ Ñ€Ð°Ð·Ð¼ÐµÑ€Ð¾Ð¼ Ð¿Ð¾Ð»Ñ Ð´Ð°Ð½Ð½Ñ‹Ñ… Ð² Ð¿Ñ€Ð¾Ñ‚Ð¾ÐºÐ¾Ð»Ðµ Ð¿ÐµÑ€ÐµÐ´Ð°Ñ‡Ð¸ Ñ‚Ð¾Ñ‡ÐµÐº)
	PAK_ERR_MEM_WR,            // 4 (Ð¾ÑˆÐ¸Ð±ÐºÐ° Ð·Ð°Ð¿Ð¸ÑÐ¸ Ð² Ð¿Ð°Ð¼ÑÑ‚ÑŒ)
	PAK_ERR_MEM_ER,            // 5 (Ð¾ÑˆÐ¸Ð±ÐºÐ° ÑÑ‚Ð¸Ñ€Ð°Ð½Ð¸Ñ Ð² Ð¿Ð°Ð¼ÑÑ‚ÑŒ)
	PAK_ERR_MEM_NOT_FREE,      // 6 (Ð¿Ð¾Ð¿Ñ‹Ñ‚ÐºÐ° Ð·Ð°Ð¿Ð¸ÑÐ¸ Ð² Ð·Ð°Ð½ÑÑ‚ÑƒÑŽ Ð¿Ð°Ð¼ÑÑ‚ÑŒ)
	PAK_ERR_DATA_OUT_OF_RANGE, // 7 (Ñ‡Ð¸ÑÐ»Ð¾ Ð²Ð½Ðµ Ð´Ð¸Ð°Ð¿Ð°Ð·Ð¾Ð½Ð°)
	PAK_ERR_UNKNOWN_FRAME_TYPE // 8 (Ð½ÐµÐ¸Ð·Ð²ÐµÑÑ‚Ð½Ñ‹Ð¹ Ñ‚Ð¸Ð¿ Ñ„Ñ€ÐµÐ¹Ð¼Ð°)
};

enum
{	                           
	PAK_PROTO_FRAME_IDENTIFY                      = 0, // ñ çåìëè íà áîðò:  çàïðîñ èäåíòèôèêàöèè óñòðîéñòâà
	PAK_PROTO_FRAME_PHOTO_STATUS                  = 1, // ñ çåìëè íà áîðò:  çàïðîñ ñòàòóñà ôîòèêà
	PAK_PROTO_FRAME_PHOTO_TEST_PICTURE            = 2, // ñ çåìëè íà áîðò:  çàïðîñ òåñòîâîãî ñíèìêà
	PAK_PROTO_FRAME_PHOTO_NEW_PICTURE             = 3, // ñ áîðòà íà çåìëþ: òî÷êà îòñíÿòà
	PAK_PROTO_FRAME_PHOTO_CLR_TAGRETS             = 4, // ñ çåìëè íà áîðò:  î÷èñòèòü ïàìÿòü öåëè
	PAK_PROTO_FRAME_PHOTO_TARGRT_LINE             = 5, // ñ çåìëè íà áîðò:  çàäàòü öåëü (ëèíèþ ñú¸ìêè è âêë. ðåæèì ñú¸ìêè ïî ëèíèÿì)
	PAK_PROTO_FRAME_PHOTO_SET_WIND_ENVIRONMENT    = 6, // 
	PAK_PROTO_FRAME_PHOTO_SET_OVERLAP_ENVIRONMENT = 7, // 
	PAK_PROTO_FRAME_PHOTO_SET_MODE                = 8,  // 
	PAK_PROTO_FRAME_RG_INFO						  = 9,	//
	PAK_PROTO_FRAME_DEL_REG_INFO				  = 10,	//
	PAK_PROTO_FRAME_PHOTO_FEEDFORWARD_DELAY		  = 11, //set the delay of photocamera, this sense use in coordinate esimator. Its time is advance 
	PAK_PROTO_FRAME_PHOTO_QTY_NON_LOAD			  = 12, //send to the 2DNPU tp show how many info of new picture not rx by 2DNPU
	PAK_PROTO_FRAME_START_EXCHNG				  = 13, //its cmd for AFS+OER version limb, Belarussia
	PAK_PROTO_FRAME_STOP_EXCHNG					  = 14,	//its cmd for AFS+OER version limb, Belarussia
	PAK_PROTO_FRAME_PHOTOCAM_ON_OFF				  = 15,	//its cmd for on/off photo
	PAK_PROTO_FRAME_SEND_ALT_LS1501				  = 16	//its cmd only for project arctica
};

#pragma pack(push, 1)


typedef	struct
{
	uint16_t type_photo;	//2
	uint16_t reserv1;		//4
	uint16_t reserv2;		//6
	uint16_t reserv3;		//8
}afs_service_info_t;

typedef	struct
{                                             // offset  size
	uint8_t  prefix[PAK_PROTO_PREFIX_SIZE];   // 0       7
	uint8_t  data_size;                       // 3       1
	uint8_t  not_data_size;                   // 4       1

	uint8_t  address;                         // 5       1
	uint8_t  frame_type;                      // 6       1
	uint16_t frame_number;                    // 7       2
} pak_proto_frame_header_t;

typedef struct
{
	pak_proto_frame_header_t  header;

	uint8_t                   buf[PAK_PROTO_MAX_FRAME_SIZE];
	int                       idx;

	int                       state;

	uint8_t*                  resync_buf;
	int                       resync_qty;

	uart_t*                   uart;

} pak_proto_handle_t;

#pragma pack(pop)


void pak_proto_init          (pak_proto_handle_t* h); // at25df_init() should be called fefore
void pak_proto_task          (pak_proto_handle_t* h);
void pak_proto_core_tx_frame (pak_proto_handle_t* h, pak_proto_frame_header_t* header, const void* data);

#endif
