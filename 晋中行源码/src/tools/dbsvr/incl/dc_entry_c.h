#ifndef __dc_entry_CH__
#define __dc_entry_CH__
struct dc_entry_c{
	char      rowid[20];
	char      entry_code[7];
	long      seqn;
	char      cur_no[5];
	char      acc_hrt[6];
	char      dc_ind[5];
	char      use_br_ind[2];
	char      ct_type[2];
	char      ct_ind[5];
	char      amt_no[41];
	char      brf[21];
};
#endif 
