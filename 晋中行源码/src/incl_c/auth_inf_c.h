#ifndef __auth_inf_CH__
#define __auth_inf_CH__
struct auth_inf_c{
	char      rowid[20];
	long      auth_no;
	char      br_no[6];
	char      tel[5];
	long      mtr_date;
	char      tx_code[5];
	double    amt;
	char      ac_no[21];
	char      auth_tel[5];
	char      auth_br_no[6];
	long      auth_date;
	char      sts[2];
	long      use_date;
	long      use_trace_no;
	double    use_amt;
	char      use_tel[5];
};
#endif 
