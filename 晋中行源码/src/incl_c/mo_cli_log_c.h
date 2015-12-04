#ifndef __mo_cli_log_CH__
#define __mo_cli_log_CH__
struct mo_cli_log_c{
	char      rowid[20];
	char      br_no[6];
	long      ac_id;
	long      ac_seqn;
	long      date;
	long      time;
	char      dc_ind[2];
	double    amt;
	char      note_knd[4];
	char      note_no[17];
	char      ct_ind[2];
};
#endif 
