#ifndef __lv_chk842_d_CH__
#define __lv_chk842_d_CH__
struct lv_chk842_d_c{
	char      rowid[20];
	char      zc_jd_lx[2];
	long      zc_date;
	char      zc_cc[3];
	char      pkgno[4];
	long      wb_cnt;
	double    wb_tx_amt;
	long      lb_cnt;
	double    lb_tx_amt;
	long      dz_wb_cnt;
	double    dz_wb_tx_amt;
	long      dz_lb_cnt;
	double    dz_lb_tx_amt;
	char      pack_sts[3];
	char      filler[61];
};
#endif 
