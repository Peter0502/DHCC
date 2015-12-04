#ifndef __mo_rmny_smny_CH__
#define __mo_rmny_smny_CH__
struct mo_rmny_smny_c{
	char      rowid[20];
	char      br_no[6];
	long      occ_date;
	long      seqn;
	char      tx_opmd[2];
	char      cur_no[3];
	double    amt;
	long      tx_date;
	long      trace_no;
	char      occ_tel[5];
	char      brf[41];
	char      tel[5];
	char      chk[5];
	char      auth[5];
	char      occ_br_no[6];
};
#endif 
