#ifndef __note_cheq_mst_CH__
#define __note_cheq_mst_CH__
struct note_cheq_mst_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seq;
	char      note_type[4];
	char      br_no[6];
	char      beg_note_no[17];
	char      end_note_no[17];
	char      book_sts[51];
	char      grp_ind[2];
	long      cnt;
	long      sell_date;
};
#endif 
