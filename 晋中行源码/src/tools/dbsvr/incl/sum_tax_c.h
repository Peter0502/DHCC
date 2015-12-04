#ifndef __sum_tax_CH__
#define __sum_tax_CH__
struct sum_tax_c{
	char      rowid[20];
	long      date;
	char      br_no[6];
	char      prdt_no[4];
	double    cls_amt;
	double    intst;
	double    tax;
	long      cnt;
};
#endif 
