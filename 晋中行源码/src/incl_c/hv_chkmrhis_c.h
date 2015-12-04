#ifndef __hv_chkmrhis_CH__
#define __hv_chkmrhis_CH__
struct hv_chkmrhis_c{
	char      rowid[20];
	long      tx_date;
	char      ccpc_no[13];
	long      send_cnt;
	double    send_amt;
	long      send_item;
	char      send_dtl[353];
	long      recevice_cnt;
	double    recevice_amt;
	long      recevice_item;
	char      recevice_dtl[353];
	long      online_cnt;
	double    online_amt;
	long      online_item;
	char      online_dtl[193];
};
#endif 
