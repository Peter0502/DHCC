#ifndef __lv_parad_CH__
#define __lv_parad_CH__
struct lv_parad_c{
	char      rowid[20];
	long      wt_date;
	long      tx_time;
	char      flg[2];
	char      chg_type[2];
	char      eff_type[2];
	long      eff_date;
	long      chg_date;
	char      par_code[33];
	char      par_name[61];
	char      par_value[251];
	char      filler[61];
};
#endif 
