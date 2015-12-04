#ifndef __mo_dc_inf_CH__
#define __mo_dc_inf_CH__
struct mo_dc_inf_c{
	char      rowid[20];
	char      acc_no_lvl1[8];
	char      acc_name_lvl1[31];
	char      acc_no_lvl2[8];
	char      acc_name_lvl2[31];
	char      acc_no_lvl3[8];
	char      acc_name_lvl3[31];
	double    dr_cash_amt;
	long      dr_cash_cnt;
	double    dr_ch_amt;
	long      dr_ch_cnt;
	double    dc_cash_amt;
	long      dc_cash_cnt;
	double    dc_ch_amt;
	long      dc_ch_cnt;
	char      sts[2];
	char      tel[5];
	char      br_no[6];
};
#endif 
