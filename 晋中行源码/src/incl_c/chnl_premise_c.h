#ifndef __chnl_premise_CH__
#define __chnl_premise_CH__
struct chnl_premise_c{
	char      rowid[20];
	long      pt_date;
	long      pt_trace_no;
	char      ac_no[20];
	long      l_premise_no;
	long      premise_no;
	double    premise_amt;
	double    finish_amt;
	char      sts[2];
	char      filler[31];
};
#endif 
