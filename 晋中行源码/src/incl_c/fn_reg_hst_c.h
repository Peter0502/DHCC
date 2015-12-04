#ifndef __fn_reg_hst_CH__
#define __fn_reg_hst_CH__
struct fn_reg_hst_c{
	char      rowid[20];
	long      tx_date;
	long      trace_no;
	char      tx_br_no[6];
	char      tel[7];
	char      type[2];
	double    tx_amt;
	char      agent_id_type[2];
	char      agent_id[21];
	char      agent_name[61];
	long      buy_date;
	long      buy_trace_no;
	char      brf[21];
	char      filler[61];
};
#endif 
