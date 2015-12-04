#ifndef __gl_prdt_dyn_hst_CH__
#define __gl_prdt_dyn_hst_CH__
struct gl_prdt_dyn_hst_c{
	char      rowid[20];
	long      date;
	char      prdt_cod[4];
	char      br_no[6];
	double    cr_bal;
	long      ac_cnt;
	double    lcd_bal;
	long      l_ac_cnt;
	long      d_opn_cnt;
	long      d_cls_cnt;
	long      rdd_cnt;
	long      rcd_cnt;
	double    rdd_amt;
	double    rcd_amt;
	long      cdd_cnt;
	long      ccd_cnt;
	double    cdd_amt;
	double    ccd_amt;
	char      spe_ind[4];
};
#endif 
