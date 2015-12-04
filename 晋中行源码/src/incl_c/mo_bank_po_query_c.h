#ifndef __mo_bank_po_query_CH__
#define __mo_bank_po_query_CH__
struct mo_bank_po_query_c{
	char      rowid[20];
	long      tx_date;
	long      trace_no;
	char      tx_br_no[6];
	long      query_date;
	char      query_br_no[13];
	char      po_no[17];
	char      query_brf[51];
	char      sts[2];
};
#endif 
