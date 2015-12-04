#ifndef __gl_tsub_CH__
#define __gl_tsub_CH__
struct gl_tsub_c{
	char      rowid[20];
	char      br_no[6];
	char      cur_no[3];
	char      acc_hrt[6];
	long      date;
	char      bal_d[2];
	double    bt_dr_bal;
	double    bt_cr_bal;
	double    r_dr_amt;
	double    r_cr_amt;
	long      r_dr_cnt;
	long      r_cr_cnt;
	double    r_dr_bal;
	double    r_cr_bal;
	char      term_type[2];
	char      spe_ind[2];
};
#endif 
