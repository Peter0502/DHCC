#ifndef __mdm_unprt_hst_CH__
#define __mdm_unprt_hst_CH__
struct mdm_unprt_hst_c{
	char      rowid[20];
	long      trace_no;
	long      trace_cnt;
	char      tx_br_no[6];
	long      ac_id;
	long      ac_seqn;
	char      add_ind[2];
	double    tx_amt;
	double    bal;
	char      tx_code[5];
	long      tx_date;
	long      hst_cnt;
	char      tel[5];
	char      brf[11];
};
#endif 
