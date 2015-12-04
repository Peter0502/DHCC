#ifndef __ln_verify_reg_CH__
#define __ln_verify_reg_CH__
struct ln_verify_reg_c{
	char      rowid[20];
	char      ac_no[22];
	long      ac_id;
	long      ac_seqn;
	char      name[52];
	char      opn_brno[7];
	double    bal_amt;
	double    in_lo_amt;
	double    out_lo_amt;
	double    cmpd_lo_amtt;
	double    rate;
	double    over_rate;
	double    fine_rate;
	long      opn_date;
	long      mtr_date;
	long      verify_date;
	long      verify_trace_no;
	char      verify_brno[7];
	char      verify_tel[6];
};
#endif 
