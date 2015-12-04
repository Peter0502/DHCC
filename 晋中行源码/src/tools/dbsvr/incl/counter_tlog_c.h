#ifndef __counter_tlog_CH__
#define __counter_tlog_CH__
struct counter_tlog_c{
	char      rowid[20];
	long      platform_date;
	double    ptrace_no;
	long      tx_date;
	long      trace_no;
	double    tx_amt;
	char      main_ac_no[20];
	char      dc_ind[2];
	char      ct_ind[2];
	char      app_ac_no[20];
};
#endif 
