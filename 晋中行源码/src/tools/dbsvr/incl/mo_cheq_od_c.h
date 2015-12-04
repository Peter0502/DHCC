#ifndef __mo_cheq_od_CH__
#define __mo_cheq_od_CH__
struct mo_cheq_od_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      note_type[4];
	char      note_no[17];
	char      brf[41];
	char      tel[5];
	long      tx_date;
	long      trace_no;
};
#endif 
