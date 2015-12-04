#ifndef __subbr_ln_CH__
#define __subbr_ln_CH__
struct subbr_ln_c{
	char      rowid[20];
	char      up_br_no[6];
	char      opn_br_no[6];
	long      ex_year;
	long      ex_month;
	long      ac_id;
	long      ac_seqn;
	char      acc_sts[2];
	double    monthb_bal;
	double    bal;
	double    mon_increment;
	double    month_addbal;
	double    season_addbal;
	double    year_addbal;
	double    true_accrual;
	double    accrual_tadd;
	double    jaccrual;
	char      client_mgr[5];
};
#endif 
