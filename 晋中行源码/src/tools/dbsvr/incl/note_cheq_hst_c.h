#ifndef __note_cheq_hst_CH__
#define __note_cheq_hst_CH__
struct note_cheq_hst_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seq;
	char      note_type[4];
	char      tx_br_no[6];
	char      beg_note_no[17];
	char      end_note_no[17];
	char      use_ind[2];
	long      cnt;
	long      tx_date;
	long      tx_time;
	long      trace_no;
	long      trace_cnt;
	char      tel[5];
	char      chk[5];
	char      auth[5];
};
#endif 
