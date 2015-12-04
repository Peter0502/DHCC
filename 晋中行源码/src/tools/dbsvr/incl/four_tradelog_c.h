#ifndef __four_tradelog_CH__
#define __four_tradelog_CH__
struct four_tradelog_c{
	char      rowid[20];
	long      tx_date;
	long      tx_time;
	char      tx_code[5];
	char      sl_bank[6];
	char      tx_tel[5];
	char      city_no[5];
	long      chang_no;
	char      cur_no[3];
	long      trace_no;
	char      ac_no[20];
	char      name[51];
	double    tx_amt;
	double    bal;
	double    sx_amt;
	double    sx_rate;
	char      cancle_trcode[5];
	long      cancle_trace_no;
	char      old_tel[5];
	char      sl_ckmk[2];
	char      fk_ckmk[2];
	char      tx_stat[2];
};
#endif 
