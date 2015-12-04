#ifndef __mo_ibl_ret_CH__
#define __mo_ibl_ret_CH__
struct mo_ibl_ret_c{
	char      rowid[20];
	char      pact_no[21];
	char      tx_br_no[6];
	double    ret_amt;
	double    ret_rate;
	long      tx_date;
	long      trace_no;
	char      sts[2];
	char      tel[5];
	char      chk[5];
	char      auth[5];
};
#endif 
