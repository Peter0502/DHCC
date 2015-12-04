#ifndef __com_item_add_CH__
#define __com_item_add_CH__
struct com_item_add_c{
	char      rowid[20];
	char      acc_no[8];
	char      acc_hrt[6];
	char      add_ind[41];
	double    dr_tx_amt;
	double    cr_tx_amt;
	long      dr_tx_cnt;
	long      cr_tx_cnt;
	double    dr_limit_amt;
	double    cr_limit_amt;
	char      brf[61];
};
#endif 
