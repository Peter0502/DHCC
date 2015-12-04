#ifndef __com_rate_CH__
#define __com_rate_CH__
struct com_rate_c{
	char      rowid[20];
	char      cur_no[3];
	char      rate_code[4];
	char      name[51];
	long      beg_date;
	long      end_date;
	double    rate_val;
	long      rate_type;
	long      rate_unit;
	char      fl_rate_code[4];
	char      ref_rate_code[4];
	double    ref_ratio;
};
#endif 
