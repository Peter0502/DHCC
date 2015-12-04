#ifndef __od_bill_CH__
#define __od_bill_CH__
struct od_bill_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      curr_cnt;
	double    amt;
	double    bal;
	double    drt_bal;
	double    intst_acm;
	double    drt_intst_acm;
	double    intst_amt;
	double    intst_bal;
	double    drt_intst_bal;
	long      beg_date;
	long      end_date;
	long      pay_date;
	long      tx_date;
	long      lst_date;
	char      pay_sts[2];
	char      curr_sts[2];
};
#endif 
