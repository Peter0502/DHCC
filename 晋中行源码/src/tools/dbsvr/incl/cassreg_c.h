#ifndef __cassreg_CH__
#define __cassreg_CH__
struct cassreg_c{
	char      rowid[20];
	char      br_no[6];
	char      db_ac_no[20];
	char      name[61];
	char      pact_no[21];
	long      bw_ac_id;
	long      in_tx_date;
	long      out_tx_date;
	double    amt;
	double    bal;
	char      tel[5];
	char      flag[2];
	long      in_trace_no;
	long      out_trace_no;
};
#endif 
