#ifndef __ln_gage_reg_CH__
#define __ln_gage_reg_CH__
struct ln_gage_reg_c{
	char      rowid[20];
	char      pact_no[21];
	char      name[61];
	long      ac_id;
	long      ac_seqn;
	char      gage_type[3];
	double    amt;
	char      gage_inf[51];
	char      sts[2];
	long      in_tx_date;
	long      in_trace_no;
	long      out_tx_date;
	long      out_trace_no;
};
#endif 
