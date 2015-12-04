#ifndef __auth_cond_def_CH__
#define __auth_cond_def_CH__
struct auth_cond_def_c{
	char      rowid[20];
	char      cond_code[4];
	char      tx_code[5];
	long      seqn;
	char      cond_mo[41];
};
#endif 
