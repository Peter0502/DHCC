#ifndef __mo_righ_invest_CH__
#define __mo_righ_invest_CH__
struct mo_righ_invest_c{
	char      rowid[20];
	long      ac_id;
	char      br_no[6];
	char      bkrl_code[4];
	char      pact_no[21];
	char      invst_type[2];
	long      num;
	double    amt;
	double    incm_bal;
	char      cif_code[21];
	char      name[51];
	long      wrk_date;
	long      trace_no;
	char      sts[2];
};
#endif 
