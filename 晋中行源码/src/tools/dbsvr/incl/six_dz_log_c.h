#ifndef __six_dz_log_CH__
#define __six_dz_log_CH__
struct six_dz_log_c{
	char      rowid[20];
	char      inf_source[2];
	long      tx_date;
	char      tx_code[5];
	char      ac_no[21];
	char      sl_bank[11];
	char      mb_bank[11];
	double    tx_amt;
	char      sl_trace_no[11];
	char      center_trace_no[11];
	char      opn_trace_no[11];
};
#endif 
