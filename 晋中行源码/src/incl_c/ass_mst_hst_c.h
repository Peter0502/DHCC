#ifndef __ass_mst_hst_CH__
#define __ass_mst_hst_CH__
struct ass_mst_hst_c{
	char      rowid[20];
	char      br_no[6];
	long      tx_date;
	long      trace_no;
	char      sub_br_no[6];
	long      pact_no;
	double    tx_amt;
	double    bal;
	double    intst_acm;
	long      tx_time;
	char      brf[11];
	long      hst_cnt;
	char      tel[5];
	char      chk[5];
	char      auth[5];
};
#endif 
