#ifndef __mo_fax_CH__
#define __mo_fax_CH__
struct mo_fax_c{
	char      rowid[20];
	long      tx_date;
	char      tx_br_no[6];
	char      fax_no[17];
	char      requ_name[51];
	char      payer_br_no[6];
	char      payer_br_name[41];
	long      payer_ac_id;
	long      payer_ac_seqn;
	char      payer_name[51];
	char      payee_br_no[6];
	char      payee_br_name[41];
	char      payee_ac_no[25];
	char      payee_name[51];
	double    fax_amt;
	char      fax_sts[2];
	long      pr_cnt;
};
#endif 
