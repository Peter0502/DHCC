#ifndef __mo_prdt_prot_CH__
#define __mo_prdt_prot_CH__
struct mo_prdt_prot_c{
	char      rowid[20];
	char      tx_br_no[6];
	char      prot_type[4];
	long      main_ac_id;
	long      main_ac_seqn;
	double    leave_amt;
	double    turn_amt;
	long      m3;
	long      m6;
	long      y1;
	long      y2;
	long      tx_date;
	long      trace_no;
	char      tel[5];
	char      sts[2];
};
#endif 
