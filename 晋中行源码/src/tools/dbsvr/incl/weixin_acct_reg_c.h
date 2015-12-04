#ifndef __weixin_acct_reg_CH__
#define __weixin_acct_reg_CH__
struct weixin_acct_reg_c{
	char      rowid[20];
	char      filename[51];
	long      acct_date;
	long      acct_time;
	char      sts[2];
	long      acct_seqn;
	char      key1[17];
	char      key2[17];
	char      filler[61];
};
#endif 
