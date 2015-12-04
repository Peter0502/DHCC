#ifndef __tx_sub_rel_tmp_CH__
#define __tx_sub_rel_tmp_CH__
struct tx_sub_rel_tmp_c{
	char      rowid[20];
	char      tx_code[5];
	char      in_sub_tx[5];
	char      out_ind[2];
	char      out_data[41];
	char      in_data[5];
	char      memo[11];
};
#endif 
