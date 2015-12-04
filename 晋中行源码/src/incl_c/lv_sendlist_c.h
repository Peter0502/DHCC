#ifndef __lv_sendlist_CH__
#define __lv_sendlist_CH__
struct lv_sendlist_c{
	char      rowid[20];
	long      tx_date;
	char      br_no[8];
	char      tel[7];
	long      trace_no;
	char      refno[21];
	char      flagno[9];
	char      packtype[4];
	char      packno[4];
	char      orderno[9];
	char      or_br_no[13];
	char      pay_qs_no[13];
	char      ac_br_no[13];
	char      cash_qs_no[13];
	double    tx_amt;
	char      respflag[2];
	char      respno[9];
	char      respdesc[61];
	char      filler[61];
};
#endif 
