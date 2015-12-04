#ifndef __com_tel_change_hst_CH__
#define __com_tel_change_hst_CH__
struct com_tel_change_hst_c{
	char      rowid[20];
	char      tel[5];
	char      br_no[6];
	char      to_tel[5];
	char      role_tel[5];
	char      sts[2];
	long      tx_date;
	long      tx_time;
	char      brf[21];
	char      filler[61];
};
#endif 
