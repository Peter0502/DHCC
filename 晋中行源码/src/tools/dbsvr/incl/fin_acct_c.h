#ifndef __fin_acct_CH__
#define __fin_acct_CH__
struct fin_acct_c{
	char      rowid[20];
	long      tx_date;
	long      fin_date;
	char      ac_no[22];
	double    amt;
	char      dc_flag[2];
	double    bal;
	char      acct_flag[2];
};
#endif 
