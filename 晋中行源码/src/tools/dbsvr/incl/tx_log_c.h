#ifndef __tx_log_CH__
#define __tx_log_CH__
struct tx_log_c{
	char      rowid[20];
	long      trace_no;
	char      tx_code[5];
	char      tx_br_no[6];
	char      bit_map[129];
	char      tx_pack[1025];
};
#endif 
