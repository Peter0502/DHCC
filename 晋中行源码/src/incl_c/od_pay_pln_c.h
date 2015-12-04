#ifndef __od_pay_pln_CH__
#define __od_pay_pln_CH__
struct od_pay_pln_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	double    od_amt;
	long      wrk_date;
	long      end_date;
	char      off_ind[2];
};
#endif 
