#ifndef __mo_trust_prot_CH__
#define __mo_trust_prot_CH__
struct mo_trust_prot_c{
	char      rowid[20];
	char      trust_no[21];
	char      tx_br_no[6];
	char      cur_no[3];
	char      name[51];
	double    trust_amt;
	double    out_amt;
	long      afund_ac_id;
	long      amt_ac_id;
	long      intst_ac_id;
	long      ln_ac_id;
	long      ln_ac_seqn;
	char      trust_ind[2];
};
#endif 
