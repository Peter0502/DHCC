#ifndef __com_exch_rate_CH__
#define __com_exch_rate_CH__
struct com_exch_rate_c{
	char      rowid[20];
	char      cur_no1[3];
	char      cur_no2[3];
	long      unit;
	double    od_buy;
	double    od_sale;
	double    dt_buy;
	double    dt_sale;
	double    mdl;
	long      date;
};
#endif 
