#ifndef __ag_unitinfo_CH__
#define __ag_unitinfo_CH__
struct ag_unitinfo_c{
	char      rowid[20];
	char      unit_no[5];
	char      unit_name[51];
	char      opr_attri[2];
	char      pay_type[3];
	char      bank_acct_no[26];
	long      beg_date;
	long      end_date;
	char      clr_flag[2];
	long      clr_date;
	char      up_unit[5];
	char      auto_flag[2];
	long      tx_date;
	long      opn_date;
	char      opn_tel[5];
	char      opn_br_no[6];
	char      unit_stat[2];
};
#endif 
