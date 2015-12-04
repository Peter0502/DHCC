#ifndef __hv_chkmrdtl_CH__
#define __hv_chkmrdtl_CH__
struct hv_chkmrdtl_c{
	char      rowid[20];
	long      tx_date;
	char      cmtno[7];
	long      s_cnt;
	double    s_amt;
	long      r_cnt;
	double    r_amt;
	char      brf[31];
};
#endif 
