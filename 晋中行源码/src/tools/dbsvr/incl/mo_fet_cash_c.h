#ifndef __mo_fet_cash_CH__
#define __mo_fet_cash_CH__
struct mo_fet_cash_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      tx_date;
	double    amt;
	long      trace_no;
	char      type[2];
};
#endif 
