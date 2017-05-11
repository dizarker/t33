/* 
 * Pak protocol slave.
 *
 */


#ifndef _PAK_PROTO_SLAVE_H_
#define _PAK_PROTO_SLAVE_H_

void pak_proto_slave_reply        (pak_proto_handle_t* h, const void* data, size_t data_size);
void pak_proto_slave_frame_parser (pak_proto_handle_t* h);


#endif
