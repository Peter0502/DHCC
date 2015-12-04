#ifndef __lv_oths_CH__
#define __lv_oths_CH__
struct lv_oths_c{
	char      rowid[20];
	char      wtday[9];
	char      orbrno[13];
	char      cmtno[4];
	char      orderno[9];
	char      stat[2];
	char      lwflg[2];
	char      brno[8];
	char      content[2001];
	char      filler[61];
};
#endif 
