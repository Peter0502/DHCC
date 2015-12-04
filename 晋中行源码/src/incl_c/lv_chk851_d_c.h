#ifndef __lv_chk851_d_CH__
#define __lv_chk851_d_CH__
struct lv_chk851_d_c{
	char      rowid[20];
	char      lw_flg[2];
	char      zc_jd_lx[2];
	long      zc_date;
	char      zc_cc[3];
	char      pkgno[4];
	long      cnt;
	char      pay_qs_no[13];
	long      pack_date;
	char      packid[9];
	long      dtlcnt;
	double    tot_amt;
	long      qs_date;
	char      pack_sts[3];
	char      dhcc_sts[3];
	char      filler[61];
};
#endif 
