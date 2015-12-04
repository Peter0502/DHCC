#ifndef __ln_pln_inf_CH__
#define __ln_pln_inf_CH__
struct ln_pln_inf_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      br_no[6];
	double    curr_amt;
	double    true_amt;
	long      wrk_date;
	long      beg_date;
	long      end_date;
	long      lst_date;
	char      pay_ind[2];
	char      content[25];
};
#endif 
