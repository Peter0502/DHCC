#ifndef __auth_chk_def_CH__
#define __auth_chk_def_CH__
struct auth_chk_def_c{
	char      rowid[20];
	char      tx_code[5];
	long      seqn;
	char      data_code[5];
	char      mo[21];
};
#endif 
