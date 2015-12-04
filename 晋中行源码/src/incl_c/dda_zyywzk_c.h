#ifndef __dda_zyywzk_CH__
#define __dda_zyywzk_CH__
struct dda_zyywzk_c{
	char      rowid[20];
	char      inst_no[8];
	char      hz_bank_no[8];
	char      ywtjbm[4];
	char      item_no[6];
	double    lst_ybal;
	double    lst_sbal;
	double    lst_mbal;
	double    lst_dbal;
	double    balance;
	double    tdy_accu;
	long      days;
	char      year[5];
	double    last_accu;
};
#endif 
