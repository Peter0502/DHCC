#ifndef __mo_sg_cash_po_CH__
#define __mo_sg_cash_po_CH__
struct mo_sg_cash_po_c{
	char      rowid[20];
	char      pact_no[21];
	char      br_no[6];
	double    pickup_sum;
	double    safeg_sum;
};
#endif 
