#ifndef __zjgl_hst_CH__
#define __zjgl_hst_CH__
struct zjgl_hst_c{
	char      rowid[20];
	char      br_no[6];
	char      cur_no[3];
	char      acc_hrt[8];
	long      date;
	double    td_dr_bal;
	double    td_cr_bal;
	double    dep_bal;
	double    cln_bal;
	double    income_bal;
	double    pay_bal;
	double    cde_in_bal;
	double    cde_out_bal;
	double    other_dr_bal;
	double    other_cr_bal;
};
#endif 
