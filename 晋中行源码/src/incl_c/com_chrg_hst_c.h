#ifndef __com_chrg_hst_CH__
#define __com_chrg_hst_CH__
struct com_chrg_hst_c{
	char      rowid[20];
	long      tx_date;
	long      trace_no;
	char      ac_no[20];
	char      tx_code[5];
	char      chrg_rate_code[4];
	char      name[31];
	double    chrg_amt;
	double    tx_amt;
	char      sts[2];
};
#endif 
