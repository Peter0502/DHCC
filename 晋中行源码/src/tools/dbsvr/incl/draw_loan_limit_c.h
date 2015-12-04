#ifndef __draw_loan_limit_CH__
#define __draw_loan_limit_CH__
struct draw_loan_limit_c{
	char      rowid[20];
	char      pact_no[21];
	char      ac_no[20];
	long      ac_id;
	double    save_amt;
	double    trust_amt;
	double    self_amt;
	double    trust_draw_amt;
	double    self_draw_amt;
	char      sts[2];
	char      br_no[6];
	char      tel[7];
	long      tx_date;
	char      brf1[61];
	char      brf2[61];
};
#endif 
