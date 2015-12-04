#ifndef __ass_rate_def_CH__
#define __ass_rate_def_CH__
struct ass_rate_def_c{
	char      rowid[20];
	char      rate_code[4];
	long      seqn;
	char      cond_ind[2];
	double    min_bal;
	double    max_bal;
	double    rate;
};
#endif 
