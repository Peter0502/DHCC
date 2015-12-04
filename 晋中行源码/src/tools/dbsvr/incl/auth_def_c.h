#ifndef __auth_def_CH__
#define __auth_def_CH__
struct auth_def_c{
	char      rowid[20];
	char      tx_code[5];
	long      seqn;
	char      cond_code[4];
	char      lvl[12];
	char      chk_ind[3];
	char      flow_code[4];
	char      mo[21];
};
#endif 
