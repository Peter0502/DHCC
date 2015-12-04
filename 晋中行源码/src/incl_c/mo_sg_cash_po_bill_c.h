#ifndef __mo_sg_cash_po_bill_CH__
#define __mo_sg_cash_po_bill_CH__
struct mo_sg_cash_po_bill_c{
	char      rowid[20];
	char      pact_no[21];
	long      bill_num;
	char      lading_bill[20];
	double    pickup_amt;
};
#endif 
