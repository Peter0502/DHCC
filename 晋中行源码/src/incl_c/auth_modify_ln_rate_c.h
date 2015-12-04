#ifndef __auth_modify_ln_rate_CH__
#define __auth_modify_ln_rate_CH__
struct auth_modify_ln_rate_c{
	char      rowid[20];
	char      auth_tel[7];
	long      auth_date;
	char      ac_no[31];
	long      ac_seqn;
	char      auth_type[2];
	char      acc_no[8];
	char      modify_value[31];
	char      tel[7];
	long      end_date;
	char      auth_sts[2];
	char      filler1[31];
	char      filler2[31];
	long      auth_no;
	char      brno[7];
	double    amt;
	char      auth_brno[7];
};
#endif 
