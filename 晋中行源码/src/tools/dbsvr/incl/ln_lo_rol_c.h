#ifndef __ln_lo_rol_CH__
#define __ln_lo_rol_CH__
struct ln_lo_rol_c{
	char      rowid[20];
	long      tx_date;
	long      trace_no;
	char      rol_ind[2];
	long      ac_id;
	long      ac_seqn;
	long      lo_term;
	double    lo_amt;
	double    lo_intst;
	double    pun_intst;
	char      pay_type[2];
	long      over_date;
	long      shld_pay_date;
	double    pay_lo_amt;
	double    pay_lo_intst;
	double    pay_pun_intst;
	char      mac[17];
	char      old_ac_ind[2];
};
#endif 
