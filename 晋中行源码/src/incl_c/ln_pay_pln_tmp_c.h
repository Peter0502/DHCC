#ifndef __ln_pay_pln_tmp_CH__
#define __ln_pay_pln_tmp_CH__
struct ln_pay_pln_tmp_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      de_ac_id;
	long      de_ac_seqn;
	double    ttl_amt;
	long      ttl_cnt;
	long      curr_cnt;
	double    curr_amt;
	double    curr_intst;
	long      wrk_date;
	long      beg_date;
	long      end_date;
	char      pay_ind[2];
};
#endif 
