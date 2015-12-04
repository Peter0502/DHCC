#ifndef __ln_paystatus_CH__
#define __ln_paystatus_CH__
struct ln_paystatus_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      pay_sts[25];
	char      clear_sts[2];
};
#endif 
