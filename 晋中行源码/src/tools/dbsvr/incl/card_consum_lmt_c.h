#ifndef __card_consum_lmt_CH__
#define __card_consum_lmt_CH__
struct card_consum_lmt_c{
	char      rowid[20];
	char      ac_no[20];
	double    da_cash_lmt;
	double    da_amt_lmt;
	double    da_con_lmt;
	double    od_cash_lmt;
	double    od_amt_lmt;
	double    mo_cash_lmt;
	double    mo_amt_lmt;
	double    mo_con_lmt;
	double    da_all_cash_lmt;
	double    da_all_amt_lmt;
	double    da_all_con_lmt;
	double    mo_all_cash_lmt;
	double    mo_all_amt_lmt;
	double    mo_all_con_lmt;
};
#endif 
