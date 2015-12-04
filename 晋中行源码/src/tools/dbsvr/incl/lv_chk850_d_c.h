#ifndef __lv_chk850_d_CH__
#define __lv_chk850_d_CH__
struct lv_chk850_d_c{
	char      rowid[20];
	char      lw_flg[2];
	char      zc_jd_lx[2];
	long      zc_date;
	char      zc_cj[3];
	char      pkgno[4];
	char      pack_sts[3];
	char      filler[61];
};
#endif 
