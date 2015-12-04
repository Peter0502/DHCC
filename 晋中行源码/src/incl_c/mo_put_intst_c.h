#ifndef __mo_put_intst_CH__
#define __mo_put_intst_CH__
struct mo_put_intst_c{
	char      rowid[20];
	char      prdt_no[4];
	long      tx_date;
	char      acc_ind[2];
	char      opn_brno[6];
	double    intst_acm;
	double    rate_val;
	double    intst;
};
#endif 
