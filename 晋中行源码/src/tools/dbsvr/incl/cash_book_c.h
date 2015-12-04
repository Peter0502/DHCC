#ifndef __cash_book_CH__
#define __cash_book_CH__
struct cash_book_c{
	char      rowid[20];
	long      trace_no;
	long      tx_date;
	char      br_no[6];
	char      tel[5];
	char      tw_br_no[6];
	char      tw_tel[5];
	char      cur_no[3];
	double    amt;
	char      ind[2];
	long      wrk_no;
	long      out_no;
};
#endif 
