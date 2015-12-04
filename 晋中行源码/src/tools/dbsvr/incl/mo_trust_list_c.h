#ifndef __mo_trust_list_CH__
#define __mo_trust_list_CH__
struct mo_trust_list_c{
	char      rowid[20];
	char      turst_pay_no[21];
	long      payer_seqn;
	char      payer_ac_no[25];
	char      payer_name[51];
	char      payer_br_no[6];
	char      payer_br_name[41];
	double    amt;
	char      sts[2];
};
#endif 
