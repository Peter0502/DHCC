#ifndef __ass_prdt_sale_sta_CH__
#define __ass_prdt_sale_sta_CH__
struct ass_prdt_sale_sta_c{
	char      rowid[20];
	char      br_no[6];
	char      prdt_code[4];
	long      date;
	long      sal_cnt;
	double    ttl_amt;
};
#endif 
