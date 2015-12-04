#ifndef __mob_acct_type_CH__
#define __mob_acct_type_CH__
struct mob_acct_type_c{
	char      rowid[20];
	char      ac_no[21];
	long      ac_id;
	long      ac_seqn;
	char      ac_type[2];
	char      server_type[5];
	char      mobile[13];
	long      tx_date;
	long      lst_fee_date;
	char      tel[7];
	char      sfbz[2];
	char      mob_sts[2];
	char      phones[61];
	char      br_no[6];
	char      filler[51];
};
#endif 
