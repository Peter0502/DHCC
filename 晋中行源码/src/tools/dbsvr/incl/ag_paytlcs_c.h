#ifndef __ag_paytlcs_CH__
#define __ag_paytlcs_CH__
struct ag_paytlcs_c{
	char      rowid[20];
	char      unit_no[5];
	char      bank_tlcs[13];
	char      batch_no[11];
	char      unit_tlcs[11];
	char      pre_no[13];
	char      bank_acct_no[26];
	char      trn_date[9];
	char      trn_time[7];
	char      br_no[8];
	char      tel_no[5];
	double    trn_amt;
	char      trcode[5];
	char      item_no[21];
	char      pay_month[7];
	char      pay_way[2];
	char      ckmk_flag[2];
	char      bill_flag[2];
	char      bill_no[11];
	char      stat[2];
};
#endif 
