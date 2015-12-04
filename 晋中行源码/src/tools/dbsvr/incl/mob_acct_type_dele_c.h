#ifndef __mob_acct_type_dele_CH__
#define __mob_acct_type_dele_CH__
struct mob_acct_type_dele_c{
	char      rowid[20];
	char      mobile[16];
	char      acct_no[21];
	char      server_type[5];
	char      trn_date[9];
	char      oper_no[5];
	char      sfbz[2];
	char      card_no[21];
	long      number;
	char      no_mark[2];
};
#endif 
