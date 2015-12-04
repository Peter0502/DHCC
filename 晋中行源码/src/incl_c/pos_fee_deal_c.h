#ifndef __pos_fee_deal_CH__
#define __pos_fee_deal_CH__
struct pos_fee_deal_c{
	char      rowid[20];
	char      fname[11];
	long      pos_date;
	char      deal_flag[2];
	long      req_cnt;
};
#endif 
