#ifndef __fn_br_quota_parm_CH__
#define __fn_br_quota_parm_CH__
struct fn_br_quota_parm_c{
	char      rowid[20];
	char      prdt_code[21];
	char      br_no[6];
	double    total_amt;
	double    available_amt;
	long      reg_date;
	char      reg_tel[7];
	long      upd_date;
	char      upd_tel[7];
};
#endif 
