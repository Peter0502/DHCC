#ifndef __mo_def_rate_CH__
#define __mo_def_rate_CH__
struct mo_def_rate_c{
	char      rowid[20];
	char      chrg_code[4];
	long      chrg_type;
	double    min_amt;
	double    max_amt;
	char      chrg_rate_no[4];
};
#endif 
