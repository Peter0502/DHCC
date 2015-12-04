#ifndef __hv_sendlist_CH__
#define __hv_sendlist_CH__
struct hv_sendlist_c{
	char      rowid[20];
	char      br_no[8];
	char      tel[7];
	long      trace_no;
	char      ref_no[21];
	char      cmtno[4];
	char      cmt_title[31];
	char      orderno[9];
	char      or_br_no[13];
	char      ac_br_no[13];
	char      pay_ac_no[33];
	char      pay_name[61];
	char      cash_ac_no[33];
	char      cash_name[61];
	double    tx_amt;
	char      resp_flg[2];
	char      resp_no[9];
	char      resp_title[61];
	char      brf[31];
};
#endif 
