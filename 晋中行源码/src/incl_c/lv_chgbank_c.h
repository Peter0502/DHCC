#ifndef __lv_chgbank_CH__
#define __lv_chgbank_CH__
struct lv_chgbank_c{
	char      rowid[20];
	char      bakcode[13];
	char      agecode[13];
	char      oagecode[13];
	char      stat[2];
	char      effday[9];
	char      filler[61];
};
#endif 
