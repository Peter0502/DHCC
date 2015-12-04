#ifndef __mo_city_query_CH__
#define __mo_city_query_CH__
struct mo_city_query_c{
	char      rowid[20];
	long      tx_date;
	long      tx_no;
	char      tx_br_no[6];
	char      tel[5];
	char      rec_br_no[6];
	char      type[2];
	long      note_date;
	char      note_type[2];
	double    tx_amt;
	char      payer_ac_no[25];
	char      payer_name[51];
	char      payer_br_name[51];
	char      payee_ac_no[25];
	char      payee_name[51];
	char      payee_br_name[51];
	char      content[51];
	char      sts[2];
};
#endif 
