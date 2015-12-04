#ifndef __in_mst_hst_CH__
#define __in_mst_hst_CH__
struct in_mst_hst_c{
	char      rowid[20];
	long      trace_no;
	long      trace_cnt;
	char      opn_br_no[6];
	char      tx_br_no[6];
	char      tx_code[5];
	char      sub_tx_code[5];
	long      tx_date;
	long      tx_time;
	long      ac_id;
	long      ac_seqn;
	char      add_ind[2];
	char      ct_ind[2];
	double    tx_amt;
	double    bal;
	double    intst_acm;
	char      note_type[4];
	char      note_no[17];
	char      brf[21];
	long      hst_cnt;
	char      tel[5];
	char      chk[5];
	char      auth[5];
};
#endif 
