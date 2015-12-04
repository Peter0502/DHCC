#ifndef __mo_hold_CH__
#define __mo_hold_CH__
struct mo_hold_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      hold_seqn;
	char      hold_typ[2];
	double    plan_hold_amt;
	char      auto_unhold_ind[2];
	char      hold_sts[2];
	long      beg_hold_date;
	long      plan_unhold_date;
	long      fct_unhold_date;
	char      hold_br_no[6];
	char      hold_tel[5];
	char      hold_brf[61];
	char      unhold_br_no[6];
	char      unhold_tel[5];
	char      unhold_brf[61];
	long      wrk_date;
	long      trace_no;
};
#endif 
