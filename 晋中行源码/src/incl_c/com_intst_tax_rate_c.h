#ifndef __com_intst_tax_rate_CH__
#define __com_intst_tax_rate_CH__
struct com_intst_tax_rate_c{
	char      rowid[20];
	char      cur_no[3];
	char      tax_rate_code[4];
	char      name[51];
	long      beg_date;
	long      end_date;
	double    tax_rate_val;
	char      filler[31];
};
#endif 
