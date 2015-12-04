#ifndef __lv_addit_CH__
#define __lv_addit_CH__
struct lv_addit_c{
	char      rowid[20];
	long      tx_date;
	char      addid[9];
	char      tagname[4];
	char      tagdata[2001];
	long      taglen;
};
#endif 
