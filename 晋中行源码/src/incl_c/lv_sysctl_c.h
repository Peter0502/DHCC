#ifndef __lv_sysctl_CH__
#define __lv_sysctl_CH__
struct lv_sysctl_c{
	char      rowid[20];
	long      tx_date;
	char      stat[2];
	long      next_date;
	char      chgnode[5];
	char      curstat[9];
	char      chgtime[15];
	char      message[61];
	char      checkstat[2];
	char      filler[101];
};
#endif 
