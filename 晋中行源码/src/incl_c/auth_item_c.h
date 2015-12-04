#ifndef __auth_item_CH__
#define __auth_item_CH__
struct auth_item_c{
	char      rowid[20];
	char      tx_code[5];
	long      seqn;
	char      code_ac[5];
	char      code_seqn[5];
	char      code_amt[5];
	char      code_dc[5];
	char      code_ct[5];
	char      code_xx[5];
	char      code_yy[5];
	char      code_zz[5];
	char      ind[5];
	char      mo[21];
};
#endif 
