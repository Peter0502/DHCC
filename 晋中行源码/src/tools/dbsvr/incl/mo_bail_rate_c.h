#ifndef __mo_bail_rate_CH__
#define __mo_bail_rate_CH__
struct mo_bail_rate_c{
	char      rowid[20];
	char      br_no[6];
	char      bail_type[3];
	char      pact_no[21];
	long      seq;
	char      bail_ac_no[25];
	char      ac_no[25];
	double    amt;
	double    ic_rate;
	long      beg_date;
	long      end_date;
	long      intst_date;
	long      in_trace_no;
	long      end_trace_no;
	double    intst_amt;
	char      sts[2];
};
#endif 
