#ifndef __pos_fee_mst_hst_CH__
#define __pos_fee_mst_hst_CH__
struct pos_fee_mst_hst_c{
	char      rowid[20];
	char      area_code[5];
	long      tx_date;
	long      tx_cnt;
	double    tx_amt;
	double    dr_amt;
	double    cr_amt;
	double    n_cr_amt;
	double    adjust_dr_amt;
	double    adjust_cr_amt;
	long      succ_num;
	double    mater_amt;
	double    clr_amt;
	long      settle_date;
	char      deal_sign[2];
	char      deal_msg[41];
};
#endif 
