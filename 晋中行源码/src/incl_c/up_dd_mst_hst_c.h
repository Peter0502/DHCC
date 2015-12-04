#ifndef __up_dd_mst_hst_CH__
#define __up_dd_mst_hst_CH__
struct up_dd_mst_hst_c{
	char      rowid[20];
	long      trace_no;
	long      trace_cnt;
	char      opn_br_no[6];
	char      tx_br_no[6];
	char      ac_no[20];
	char      sub_ac_no[20];
	char      add_ind[2];
	char      ct_ind[2];
	double    tx_amt;
	double    bal;
	double    intst_acm;
	long      tx_date;
	long      tx_time;
	long      hst_cnt;
	char      brf[21];
	char      tel[5];
	char      chk[5];
	char      auth[5];
};
#endif 
