#ifndef __com_def_chrg_CH__
#define __com_def_chrg_CH__
struct com_def_chrg_c{
	char      rowid[20];
	char      tx_code[5];
	long      seqn;
	char      chrg_rate_code[4];
	char      chrg_ac_no[5];
	char      chrg_mode[2];
	char      get_ind[2];
	char      cond[41];
	char      amt_no[41];
	char      mo[21];
};
#endif 
