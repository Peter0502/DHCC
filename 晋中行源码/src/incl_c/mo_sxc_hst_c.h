#ifndef __mo_sxc_hst_CH__
#define __mo_sxc_hst_CH__
struct mo_sxc_hst_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      rate_date;
	long      ic_date;
	long      tx_date;
	long      trace_no;
	double    tx_amt;
	double    hst_bal;
	double    intst;
	char      opn_br_no[6];
	char      tx_br_no[6];
	char      tx_tel[7];
	char      type[2];
	char      sts[2];
};
#endif 
