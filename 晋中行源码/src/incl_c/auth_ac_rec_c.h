#ifndef __auth_ac_rec_CH__
#define __auth_ac_rec_CH__
struct auth_ac_rec_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      dc[2];
	char      ct[2];
	char      ind[5];
	char      tx_code[5];
	double    amt;
};
#endif 
