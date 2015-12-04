#ifndef __lv_czyh_dz_CH__
#define __lv_czyh_dz_CH__
struct lv_czyh_dz_c{
	char      rowid[20];
	char      lw_ind[2];
	long      pack_date;
	char      pack_id[9];
	char      pack_type[4];
	long      check_cnt;
	double    check_amt;
	char      check_flag[2];
	char      pay_qs_no[13];
	long      tx_date;
	char      br_no[6];
};
#endif 
