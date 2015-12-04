#ifndef __gl_mst_hst_CH__
#define __gl_mst_hst_CH__
struct gl_mst_hst_c{
	char      rowid[20];
	char      br_no[6];
	char      cur_no[3];
	char      acc_hrt[6];
	long      date;
	char      dc_ind[2];
	char      up_acc_hrt[6];
	double    dr_bal;
	double    cr_bal;
	double    ldd_bal;
	double    lcd_bal;
	long      rdd_cnt;
	long      rcd_cnt;
	double    rdd_amt;
	double    rcd_amt;
	long      cdd_cnt;
	long      ccd_cnt;
	double    cdd_amt;
	double    ccd_amt;
};
#endif 
