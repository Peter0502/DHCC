#ifndef __ln_eod_pri_CH__
#define __ln_eod_pri_CH__
struct ln_eod_pri_c{
	char      rowid[20];
	char      ln_ac_no[21];
	long      ln_ac_seqn;
	char      ln_ac_name[51];
	char      ln_pact_no[21];
	char      ln_ac_prdt[4];
	char      pay_ac_no[21];
	long      pay_ac_seqn;
	char      pay_ac_name[51];
	double    amt;
	double    acm;
	double    rate;
	double    over_rate;
	char      tx_code[5];
	char      amt_type[2];
	char      acm_type[2];
	long      beg_date;
	long      end_date;
	long      trace_no;
	long      tx_date;
};
#endif 
