#ifndef __mo_cashier_CH__
#define __mo_cashier_CH__
struct mo_cashier_c{
	char      rowid[20];
	char      sign_br_no[6];
	char      cashier_che_no[17];
	char      tx_br_no[6];
	char      ct_ind[2];
	char      ration_ind[2];
	char      requ_name[51];
	char      requ_addr[51];
	long      requ_ac_id;
	long      requ_ac_seqn;
	char      requ_br_no[6];
	char      requ_br_name[41];
	long      sign_date;
	char      payee_name[51];
	double    sign_amt;
	char      purpose[41];
	char      rmk[41];
	char      cashier_sts[2];
	char      cashier_che_fag[2];
};
#endif 
