#ifndef __com_chrg_rate_CH__
#define __com_chrg_rate_CH__
struct com_chrg_rate_c{
	char      rowid[20];
	char      cur_no[3];
	char      chrg_rate_code[4];
	long      beg_date;
	long      end_date;
	char      dc_code[5];
	char      name[21];
	char      type[2];
	double    val;
	double    min_amt;
	double    max_amt;
};
#endif 
