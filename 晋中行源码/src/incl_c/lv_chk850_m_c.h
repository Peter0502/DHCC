#ifndef __lv_chk850_m_CH__
#define __lv_chk850_m_CH__
struct lv_chk850_m_c{
	char      rowid[20];
	long      in_date;
	long      pack_date;
	long      dz_date;
	char      cash_qs_no[13];
	long      qs_zt_cnt;
	long      qt_zt_cnt;
	char      filler[61];
};
#endif 
