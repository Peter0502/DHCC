#ifndef __draw_loan_hst_CH__
#define __draw_loan_hst_CH__
struct draw_loan_hst_c{
	char      rowid[20];
	char      st_flag[2];
	char      pact_no[21];
	char      draw_no[25];
	char      ac_no[21];
	long      ac_id;
	double    draw_amt;
	char      approve_tel[7];
	long      approve_date;
	char      sts[2];
	char      tel[7];
	long      trace_no;
	long      tx_date;
	long      ejfno;
	char      trust_ac_no[33];
	char      trust_name[61];
	char      opn_br_name[61];
	char      brf1[61];
	char      brf2[61];
};
#endif 
