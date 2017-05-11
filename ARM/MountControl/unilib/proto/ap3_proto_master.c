#include "include.h"




//=============================================================================
void ap3_proto_master_tx(ap3_proto_frame_t* frame, ap3_proto_pckt_type6_t* answer)
{
	STATIC_ASSERT(sizeof(ap3_proto_pckt_type6_t)==8);
		
	pin_init(ALT_PIN_UART1_TX);
		
	frame->header.all_size 			= 5 + sizeof(ap3_proto_pckt_type6_t);
	frame->header.not_all_size 		= ~frame->header.all_size;
	frame->header.talker_id_bendian	= 0x0300; //talker_id = 3, its Logger id, but need repack to big endiane
	frame->header.packet_type	 	= 0x06;
		
	memcpy(&frame->data[0], answer, sizeof(ap3_proto_pckt_type6_t));
	
	size_t size_data = sizeof(ap3_proto_pckt_type6_t);
	
	frame->data[size_data] = crc8(&frame, frame->header.all_size);
	
	uart_put_block(&AP3_PROTO_UART, &frame->header, sizeof(ap3_proto_frame_header_t));
	uart_put_block(&AP3_PROTO_UART, &frame->data[0],  size_data+1);
	
   	while(fifo_get_qty(&uart1.fifo.tx))
	{}
	delay_ms(10);
	pin_init(PIN_UART1_TX); 
	pin_clr(PIN_UART1_TX);
		
}

