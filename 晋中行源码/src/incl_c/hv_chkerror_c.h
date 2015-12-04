#ifndef __hv_chkerror_CH__
#define __hv_chkerror_CH__
struct hv_chkerror_c{
	char      rowid[20];
	char      br_no[8];
	char      cmtno[4];
	char      orderno[9];
	char      pay_qs_no[13];
	char      or_br_no[13];
	long      tx_date;
	long      tx_time;
	double    tx_amt;
	double    tx_amt_ccb;
	double    tx_amt_ccpc;
	char      o_sts[2];
	char      sts[2];
	char      chk_flg[2];
	char      title[31];
	char      prt_flag[2];
	char      brf[61];
};
#endif 
