#ifndef __ln_acm_reg_CH__
#define __ln_acm_reg_CH__
struct ln_acm_reg_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      br_no[6];
	long      beg_date;
	long      end_date;
	long      cal_date;
	double    rate;
	double    intst_acm;
	double    part_intst_acm;
	double    intst;
	char      intst_type[2];
	long      trace_no;
	char      type[2];
};
#endif 
