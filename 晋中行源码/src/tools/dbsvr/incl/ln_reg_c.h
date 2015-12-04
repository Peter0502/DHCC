#ifndef __ln_reg_CH__
#define __ln_reg_CH__
struct ln_reg_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      reg_date_beg;
	long      reg_date_end;
	long      pay_date;
	char      type[2];
	double    lo_bal;
	double    acm;
	double    pay_bal;
	char      ind[2];
	char      sts[2];
};
#endif 
