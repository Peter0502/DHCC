#ifndef __mo_pay_again_ln_CH__
#define __mo_pay_again_ln_CH__
struct mo_pay_again_ln_c{
	char      rowid[20];
	char      pact_no[17];
	double    amt;
	long      pay_date;
	double    intst_amt;
	char      sts[2];
};
#endif 
