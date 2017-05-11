/* 
 * Pak protocol master.
 *
 */

#ifndef _PAK_PROTO_MASTER_H_
#define _PAK_PROTO_MASTER_H_


#define PAK_PROTO_MASTER_POOL_SIZE      8                    // power of 2
#define PAK_PROTO_MASTER_BUF_SIZE       sizeof(pic_info_full_t)


void pak_proto_master_init              (void); // at25df_init() should be called fefore

void pak_proto_master_task_2hz          (pak_proto_handle_t* h);
void pak_proto_master_task              (pak_proto_handle_t* h);

void pak_proto_master_frame_parser      (pak_proto_handle_t* h);
void pak_proto_master_tx_frame_with_ack (uint_fast8_t frame_type, void* data, size_t data_size);
void pak_proto_master_tx_info_qty_non_load_photo(uint16_t frame_type);
#endif
