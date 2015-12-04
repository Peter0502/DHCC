#ifndef __ln_expand_CH__
#define __ln_expand_CH__
struct ln_expand_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      exp_br_no[6];
	double    exp_amt;
	double    exp_rate;
	long      exp_beg_date;
	long      exp_mtr_date;
	char      exp_ind[2];
	long      rgst_date;
	char      tel[5];
	long      trace_no;
};
#endif 
