#ifndef __od_mst_hst_CH__
#define __od_mst_hst_CH__
struct od_mst_hst_c{
	char      rowid[20];
	long      trace_no;
	long      trace_cnt;
	char      opn_br_no[6];
	char      tx_br_no[6];
	long      ac_id;
	long      ac_seqn;
	char      tx_code[5];
	char      sub_tx_code[5];
	char      add_ind[2];
	char      ct_ind[2];
	double    tx_bal;
	double    bal;
	double    intst_acm;
	long      tx_date;
	long      tx_time;
	char      note_type[4];
	char      note_no[17];
	long      hst_cnt;
	char      brf[21];
	char      tel[5];
	char      chk[5];
	char      auth[5];
};
#endif 
