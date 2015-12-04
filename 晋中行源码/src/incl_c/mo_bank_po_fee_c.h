#ifndef __mo_bank_po_fee_CH__
#define __mo_bank_po_fee_CH__
struct mo_bank_po_fee_c{
	char      rowid[20];
	char      pact_no[21];
	long      num;
	double    po_amt;
	double    fee_ratio;
	double    fee_amt;
	double    prm_amt;
	char      sts[2];
	long      new_num;
};
#endif 
