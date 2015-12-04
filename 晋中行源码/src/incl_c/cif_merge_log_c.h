#ifndef __cif_merge_log_CH__
#define __cif_merge_log_CH__
struct cif_merge_log_c{
	char      rowid[20];
	long      cif_no_old;
	long      cif_no_new;
	long      tx_date;
	long      tx_time;
	char      tel[5];
	long      log_cnt;
};
#endif 
