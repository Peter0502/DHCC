#ifndef __chnl_tradeadd_CH__
#define __chnl_tradeadd_CH__
struct chnl_tradeadd_c{
	char      rowid[20];
	long      tx_date;
	char      ac_no[21];
	char      tx_type[2];
	double    add_amt;
	long      add_qk_cnt;
	long      add_pswd_cnt;
	char      stat[2];
};
#endif 
