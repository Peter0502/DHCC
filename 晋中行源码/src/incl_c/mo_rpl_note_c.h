#ifndef __mo_rpl_note_CH__
#define __mo_rpl_note_CH__
struct mo_rpl_note_c{
	char      rowid[20];
	char      old_ac_no[20];
	char      br_no[6];
	char      mdm_code[5];
	char      old_note_no[17];
	char      new_ac_no[20];
	char      new_note_no[17];
	long      rpl_date;
	char      tel[5];
	long      trace_no;
	char      rmk[2];
};
#endif 
