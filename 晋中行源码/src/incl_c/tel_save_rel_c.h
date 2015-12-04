#ifndef __tel_save_rel_CH__
#define __tel_save_rel_CH__
struct tel_save_rel_c{
	char      rowid[20];
	char      tel[5];
	char      ac_no[20];
	long      ac_seqn;
	long      cif_no;
	char      tx_type[2];
	char      sts[2];
	long      tx_date;
	long      tx_time;
	char      tx_tel[5];
	char      br_no[6];
	long      trace_no;
	char      brf[31];
};
#endif 
