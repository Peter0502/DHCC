#ifndef __auth_cond_CH__
#define __auth_cond_CH__
struct auth_cond_c{
	char      rowid[20];
	char      type[2];
	long      no;
	char      tx_code[5];
	long      seqn;
	char      mo[21];
	char      cond[101];
};
#endif 
