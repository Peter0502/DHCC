#ifndef __tx_def_CH__
#define __tx_def_CH__
struct tx_def_c{
	char      rowid[20];
	char      tx_code[5];
	char      tx_name[21];
	char      bit_map[129];
	char      log_ind[2];
	char      tx_type[2];
	char      wrk_sts[2];
};
#endif 
