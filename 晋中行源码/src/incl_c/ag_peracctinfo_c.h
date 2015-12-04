#ifndef __ag_peracctinfo_CH__
#define __ag_peracctinfo_CH__
struct ag_peracctinfo_c{
	char      rowid[20];
	char      per_no[13];
	char      per_name[51];
	char      per_stat[2];
	char      acct_type[2];
	char      bank_acct_no[26];
	char      per_pwd[7];
	char      opn_br_no[8];
	char      opn_date[9];
	char      opn_tel[5];
	char      cls_date[9];
	char      cls_tel[5];
};
#endif 
