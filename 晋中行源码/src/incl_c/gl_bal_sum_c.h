#ifndef __gl_bal_sum_CH__
#define __gl_bal_sum_CH__
struct gl_bal_sum_c{
	char      rowid[20];
	char      br_no[6];
	char      cur_no[3];
	char      acc_hrt[6];
	long      date;
	char      dc_ind[2];
	char      up_acc_hrt[6];
	double    dr_bal;
	double    cr_bal;
	long      ac_cnt;
	double    ldd_bal;
	double    lcd_bal;
	long      lac_cnt;
	long      rdd_cnt;
	long      rcd_cnt;
	double    rdd_amt;
	double    rcd_amt;
	long      cdd_cnt;
	long      ccd_cnt;
	double    cdd_amt;
	double    ccd_amt;
	double    tddr_bal;
	double    tdcr_bal;
	long      tdac_cnt;
	long      tddr_cnt;
	long      tdcr_cnt;
	double    tddr_amt;
	double    tdcr_amt;
	double    mdr_bal;
	double    mcr_bal;
	long      mac_cnt;
	long      mdr_cnt;
	long      mcr_cnt;
	double    mdr_amt;
	double    mcr_amt;
	double    qdr_bal;
	double    qcr_bal;
	long      qac_cnt;
	long      qdr_cnt;
	long      qcr_cnt;
	double    qdr_amt;
	double    qcr_amt;
	double    ydr_bal;
	double    ycr_bal;
	long      yac_cnt;
	long      ydr_cnt;
	long      ycr_cnt;
	double    ydr_amt;
	double    ycr_amt;
};
#endif 
