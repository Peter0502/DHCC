#ifndef __hv_errormr_CH__
#define __hv_errormr_CH__
struct hv_errormr_c{
	char      rowid[20];
	char      br_no[8];
	char      cmtno[4];
	char      orderno[9];
	char      pay_qs_no[13];
	long      tx_date;
	long      tx_time;
	double    tx_amt_ccb;
	double    tx_amt_ccpc;
	char      sts[2];
	char      flg[2];
	char      brf[61];
};
#endif 
