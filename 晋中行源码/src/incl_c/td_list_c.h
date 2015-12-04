#ifndef __td_list_CH__
#define __td_list_CH__
struct td_list_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      opn_br_no[6];
	long      tx_date;
	double    td_rate;
	double    dd_rate;
	char      flag[2];
	long      td_day;
	long      dd_day;
	double    td_lx;
	double    dd_lx;
	double    td_amt;
	double    dd_amt;
};
#endif 
