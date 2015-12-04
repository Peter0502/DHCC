#ifndef __mo_unchange_CH__
#define __mo_unchange_CH__
struct mo_unchange_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      in_acno[20];
	double    bal;
	double    intst;
	char      ind[3];
	char      prdt_no[6];
	long      trace_no;
	char      tx_br_no[6];
	long      tx_date;
	long      tx_time;
	char      brf[31];
	char      tel[5];
};
#endif 
