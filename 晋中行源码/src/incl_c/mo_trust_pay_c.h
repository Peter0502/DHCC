#ifndef __mo_trust_pay_CH__
#define __mo_trust_pay_CH__
struct mo_trust_pay_c{
	char      rowid[20];
	char      br_no[6];
	char      turst_pay_no[11];
	char      turst_fag[2];
	char      requ_name[51];
	char      requ_id_type[2];
	char      requ_id_no[21];
	long      payee_ac_id;
	long      payee_ac_seqn;
	char      sts[2];
};
#endif 
