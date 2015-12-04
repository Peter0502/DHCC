#ifndef __lv_c503r_CH__
#define __lv_c503r_CH__
struct lv_c503r_c{
	char      rowid[20];
	char      bakcode[13];
	long      sendcnt;
	char      sendgrant[2001];
	long      recvcnt;
	char      recvgrant[2001];
	char      upd_date[9];
	char      upd_node[5];
};
#endif 
