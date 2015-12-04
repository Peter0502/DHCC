#ifndef __card_book_CH__
#define __card_book_CH__
struct card_book_c{
	char      rowid[20];
	char      mdm_code[5];
	char      beg_no[21];
	char      end_no[21];
	long      cnt;
	long      tx_date;
	char      br_no[6];
	char      tel[5];
};
#endif 
