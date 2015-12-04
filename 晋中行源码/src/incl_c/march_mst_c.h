#ifndef __march_mst_CH__
#define __march_mst_CH__
struct march_mst_c{
	char      rowid[20];
	char      area_code[5];
	char      march_code[16];
	char      ac_no[21];
	char      march_name[41];
	long      tx_date;
	long      tx_cnt;
	double    tx_amt;
	double    fee_amt;
	double    fee_rate;
	double    real_amt;
	long      acct_date;
	char      deal_sign[2];
	char      deal_msg[41];
	char      opn_br_no[6];
	char      tt_no[5];
};
#endif 
