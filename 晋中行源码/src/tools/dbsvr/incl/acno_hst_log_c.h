#ifndef __acno_hst_log_CH__
#define __acno_hst_log_CH__
struct acno_hst_log_c{
	char      rowid[20];
	char      ac_type[2];
	long      ac_id;
	long      ac_seqn;
	long      hst_cnt;
	long      tx_date;
	char      br_no[6];
	char      note_type[4];
	char      note_no[17];
	char      tel[5];
	double    tx_amt;
	double    bal;
	char      dc_ind[2];
	char      print_flag[2];
	long      pagecnt;
	char      brf[21];
	char      filler[21];
	char      filler2[41];
};
#endif 
