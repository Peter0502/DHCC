#ifndef __ag_unitacctmap_CH__
#define __ag_unitacctmap_CH__
struct ag_unitacctmap_c{
	char      rowid[20];
	char      br_no[8];
	char      unit_no[5];
	char      acct_type[2];
	char      bank_acct_no[26];
	char      stat[2];
};
#endif 
