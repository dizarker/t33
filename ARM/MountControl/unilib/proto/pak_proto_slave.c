#include "include.h"

//=============================================================================
void pak_proto_slave_reply(pak_proto_handle_t* h, const void* data, size_t data_size)
{
	if (h->header.address & PAK_PROTO_ACK_BIT_MASK)
	{
	//	h->header.frame_number;                          // echo
	//	h->header.frame_type;                            // echo

		h->header.address   &= ~PAK_PROTO_ACK_BIT_MASK;  // w/o ACK because of it is replay already
		h->header.data_size  =  data_size;

		pak_proto_core_tx_frame(h, &h->header, data);
	}
}

//=============================================================================
static void pak_proto_slave_reply_error(pak_proto_handle_t* h, uint_fast8_t err_code)   // puts byte of err_code instead of data field
{
	static const size_t ERR_CODE_SIZE = 1;           // one byte of err_code

	pak_proto_slave_reply(h, &err_code, ERR_CODE_SIZE);
}

//=============================================================================
void pak_proto_slave_frame_parser(pak_proto_handle_t* h)
{
#ifdef LIMB_JAVAD_CM_V2_SVN5735
		pin_toggle(PIN_LED_GREEN);
#endif
	
	switch (h->header.frame_type)
	{
		
		case PAK_PROTO_FRAME_IDENTIFY:
		{						
			pak_proto_slave_reply(h, &afs_service_info, sizeof(afs_service_info_t));
			break;
		}

		case PAK_PROTO_FRAME_PHOTO_STATUS:
		{						
			#ifdef 	CANON_650_D_UNIVERSAL
				pin_init(PIN_UART1_TX);
				PHOTO_USB2_VCC_OFF();
			#endif
			
			STATIC_ASSERT(sizeof(photo.status)==52);
			pak_proto_slave_reply(h, &photo.status, sizeof(photo.status));
			break;
		}

		case PAK_PROTO_FRAME_PHOTO_TEST_PICTURE:
		{						

			#ifdef SONY_QX_100
				qx100_cmd_on(&qx100_control);
				qx100_off_after_test_photo_timer.update_end_sense 	= 1;
				qx100_off_after_test_photo_timer.stop 				= 0;
				if(qx100_control.state) photo_take_test_picture();				
			#else
				photo_take_test_picture();
			#endif
			break;
		}

		case PAK_PROTO_FRAME_PHOTO_CLR_TAGRETS:
		{
			pak_proto_slave_reply_error(h, map_storage_format() ?  PAK_OK : PAK_ERR_MEM_ER);
			break;
		}

		case PAK_PROTO_FRAME_PHOTO_TARGRT_LINE:
		{
			uint_fast8_t err_code = map_storage_feed(h->header.frame_number, &h->buf[PAK_PROTO_HEADER_SIZE], h->header.data_size);

			pak_proto_slave_reply_error(h, err_code);
						
			break;
		}

		case PAK_PROTO_FRAME_PHOTO_SET_WIND_ENVIRONMENT:
		{
			uint_fast8_t err_code = photo_set_wind_environment(&h->buf[PAK_PROTO_HEADER_SIZE], h->header.data_size);

			pak_proto_slave_reply_error(h, err_code);
			break;
		}

		case PAK_PROTO_FRAME_PHOTO_SET_OVERLAP_ENVIRONMENT:
		{
			uint_fast8_t err_code = photo_set_overlap_environment(&h->buf[PAK_PROTO_HEADER_SIZE], h->header.data_size);

			pak_proto_slave_reply_error(h, err_code);
			break;
		}

		case PAK_PROTO_FRAME_PHOTO_SET_MODE:
		{
			uint_fast8_t err_code = photo_set_mode(&h->buf[PAK_PROTO_HEADER_SIZE], h->header.data_size);

			pak_proto_slave_reply_error(h, err_code);
			break;
		}
		
		case PAK_PROTO_FRAME_RG_INFO:
		{
			uint_fast8_t err_code = region_parser(h->header.frame_number, &h->buf[PAK_PROTO_HEADER_SIZE], h->header.data_size);
			
			pak_proto_slave_reply_error(h, err_code);
			break;
		}
		case PAK_PROTO_FRAME_DEL_REG_INFO:
		{
			uint_fast8_t err_code = delete_all_region();
			
			pak_proto_slave_reply_error(h, err_code);
			break;
		}
		case PAK_PROTO_FRAME_PHOTO_FEEDFORWARD_DELAY:
		{
			#ifdef SONY_QX_100
				if(qx100_control.state) photo_take_feedforward_picture();
			#else
				photo_take_feedforward_picture();
			#endif
			break;
		}
	
		case PAK_PROTO_FRAME_PHOTOCAM_ON_OFF:
		{			
			uint_fast8_t err_code = 0;	//init special this cmd can use by another photo
			
			#ifdef SONY_QX_100
				err_code = qx100_set_onoff_manual(h->header.frame_number, &h->buf[PAK_PROTO_HEADER_SIZE], h->header.data_size);
			#endif
			pak_proto_slave_reply_error(h, err_code);
			
			break;
		}
	
	
	#ifdef 	CANON_650_D_UNIVERSAL
		
		case PAK_PROTO_FRAME_START_EXCHNG:
		{
			pin_init(PIN_UART1_TX);
			PHOTO_USB2_VCC_OFF();
			pak_proto_slave_reply_error(h, 0);
			break;
		}
		
		case PAK_PROTO_FRAME_STOP_EXCHNG:
		{			
			pin_init(PIN_UART1_TX);
			PHOTO_USB2_VCC_OFF();
			
			pak_proto_slave_reply_error(h, 0);
			delay_ms(50);
			
			pin_init(PIN_UART1_TX_HI_ZERO);
			PHOTO_USB2_VCC_ON();
			break;
		}
	#endif
		default:
		{
			pak_proto_slave_reply_error(h, PAK_ERR_UNKNOWN_FRAME_TYPE);
			break;
		}
	}
}

