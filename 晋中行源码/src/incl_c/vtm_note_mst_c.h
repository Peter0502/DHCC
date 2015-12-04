#ifndef __vtm_note_mst_CH__
#define __vtm_note_mst_CH__
struct vtm_note_mst_c{
	char      rowid[20];
	char      tel[5];
	char      tw_tel[5];
	char      note_type[4];
	char      beg_note_no1[17];
	char      end_note_no1[17];
	char      beg_note_no2[17];
	char      end_note_no2[17];
	char      beg_note_no3[17];
	char      end_note_no3[17];
	char      ind[2];
	long      tx_date;
	long      trace_no;
};
#endif 
