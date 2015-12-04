#ifndef __remark_ctl_CH__
#define __remark_ctl_CH__
struct remark_ctl_c{
	char      rowid[20];
	char      br_no[6];
	char      ac_no[25];
	long      ac_id;
	long      ac_seqn;
	char      rem_code[11];
	double    amt;
	char      rem[51];
	long      tx_date;
	char      tx_tel[5];
	long      lst_date;
	char      lst_tel[5];
	char      rem_sts[2];
};
#endif 
