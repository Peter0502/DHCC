#ifndef __fn_br_profit_CH__
#define __fn_br_profit_CH__
struct fn_br_profit_c{
	char      rowid[20];
	char      prdt_code[21];
	char      br_no[6];
	double    tx_amt;
	double    redempt_amt;
	double    profit_amt;
	long      reg_date;
	long      reg_trace_no;
	long      pay_date;
	long      pay_trace_no;
	char      sts[2];
	char      brf[21];
	char      filler[61];
};
#endif 
