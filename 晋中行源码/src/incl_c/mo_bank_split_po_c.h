#ifndef __mo_bank_split_po_CH__
#define __mo_bank_split_po_CH__
struct mo_bank_split_po_c{
	char      rowid[20];
	char      pact_no[13];
	char      tx_br_no[6];
	long      req_date;
	char      payer_name[51];
	char      payer_ac_no[33];
	char      payer_br_name[41];
	char      payee_name[51];
	char      payee_ac_no[33];
	char      payee_br_name[41];
	char      pnote_no[13];
	double    par_amt;
	long      matr_date;
	char      bail_ac_no[25];
	char      pay_type[2];
	char      sts[2];
};
#endif 
