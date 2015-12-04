#ifndef __march_mst_mx_hst_CH__
#define __march_mst_mx_hst_CH__
struct march_mst_mx_hst_c{
	char      rowid[20];
	char      area_code[5];
	char      march_code[16];
	char      tty_no[9];
	long      tx_time;
	long      tx_date;
	char      brf[21];
	long      trace_no;
	char      card_no[20];
	char      opn_tx_br_name[41];
	double    tx_amt;
};
#endif 
