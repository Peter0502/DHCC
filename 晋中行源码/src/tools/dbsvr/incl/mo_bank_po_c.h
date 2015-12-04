#ifndef __mo_bank_po_CH__
#define __mo_bank_po_CH__
struct mo_bank_po_c{
	char      rowid[20];
	char      sign_br_no[6];
	long      sign_date;
	long      receive_date;
	long      cash_date;
	char      pact_no[21];
	long      pact_no_num;
	char      po_no[31];
	double    po_amt;
	char      at_term_ind[2];
	char      po_sts[2];
	char      po_ind[2];
};
#endif 
