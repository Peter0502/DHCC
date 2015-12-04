#ifndef __lv_newxyh_CH__
#define __lv_newxyh_CH__
struct lv_newxyh_c{
	char      rowid[20];
	char      xyno[61];
	char      pay_ac_no[33];
	char      pay_name[61];
	char      pay_no[13];
	char      cashxyno[61];
	char      cash_ac_no[33];
	char      cash_name[61];
	char      cash_no[13];
	char      yw_type[6];
	char      ywlist[201];
	double    limitamt;
	char      br_no[8];
	long      last_date;
	char      tel[7];
	char      flag[2];
	char      filler[61];
};
#endif 
