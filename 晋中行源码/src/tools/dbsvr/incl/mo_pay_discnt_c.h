#ifndef __mo_pay_discnt_CH__
#define __mo_pay_discnt_CH__
struct mo_pay_discnt_c{
	char      rowid[20];
	char      pact_no[21];
	char      tpact_no[21];
	char      rpact_no[21];
	long      ac_id;
	long      ac_seqn;
	char      ct_ind[2];
	double    pay_amt;
	char      turn_ac_no[25];
	char      turn_name[51];
	char      ac_type[2];
	double    turn_bal;
	char      in_ac_no[11];
	char      tin_ac_no[11];
	char      rin_ac_no[11];
	char      sts[2];
	char      pn_type[2];
};
#endif 
