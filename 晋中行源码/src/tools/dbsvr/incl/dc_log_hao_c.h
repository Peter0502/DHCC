#ifndef __dc_log_hao_CH__
#define __dc_log_hao_CH__
struct dc_log_hao_c{
	char      rowid[20];
	long      tx_date;
	long      trace_no;
	long      trace_cnt;
	char      entry_code[7];
	long      dc_seqn;
	char      opn_br_no[6];
	char      tx_br_no[6];
	char      tx_opn_br_no[6];
	char      tx_tx_br_no[6];
	char      cur_no[3];
	char      acc_hrt[6];
	char      dc_ind[2];
	char      ct_ind[2];
	char      note_type[4];
	char      note_no[17];
	double    amt;
	char      tel[5];
	char      sts[2];
};
#endif 
