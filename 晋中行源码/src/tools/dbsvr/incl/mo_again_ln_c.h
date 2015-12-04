#ifndef __mo_again_ln_CH__
#define __mo_again_ln_CH__
struct mo_again_ln_c{
	char      rowid[20];
	char      pact_no[17];
	long      ac_id;
	char      br_no[6];
	char      prdt_no[4];
	double    amt;
	double    cur_amt;
	double    sum_amt;
	double    rate;
	long      limit;
	long      to_date;
	long      reg_date;
	long      pay_date;
	char      tel[5];
	char      chk[5];
	char      auth[5];
	char      again_ln_type[2];
	char      sts[2];
};
#endif 
