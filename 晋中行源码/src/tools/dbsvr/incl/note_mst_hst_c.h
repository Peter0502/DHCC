#ifndef __note_mst_hst_CH__
#define __note_mst_hst_CH__
struct note_mst_hst_c{
	char      rowid[20];
	char      tel[5];
	char      note_type[4];
	char      br_no[6];
	char      beg_note_no[17];
	char      end_note_no[17];
	long      note_cnt;
	long      tx_date;
	long      tx_time;
	long      trace_no;
	long      trace_cnt;
	char      auth[6];
	char      tw_br_no[6];
	char      tw_tel[6];
	char      sts_ind[2];
};
#endif 
