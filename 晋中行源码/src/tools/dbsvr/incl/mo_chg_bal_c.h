#ifndef __mo_chg_bal_CH__
#define __mo_chg_bal_CH__
struct mo_chg_bal_c{
	char      rowid[20];
	char      br_no[6];
	long      ac_id;
	long      tx_date;
	long      ac_seqn;
	double    y_bal;
	double    by_bal;
	double    chg_bal;
};
#endif 
