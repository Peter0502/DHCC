#ifndef __subbr_deposit_hst_CH__
#define __subbr_deposit_hst_CH__
struct subbr_deposit_hst_c{
	char      rowid[20];
	char      up_br_no[6];
	char      opn_br_no[6];
	long      ex_year;
	long      ex_month;
	long      ac_id;
	long      ac_seqn;
	double    monthb_bal;
	double    bal;
	double    mon_increment;
	double    month_addbal;
	double    season_addbal;
	double    year_addbal;
	char      draw_deposit[5];
	char      client_mgr[5];
};
#endif 
