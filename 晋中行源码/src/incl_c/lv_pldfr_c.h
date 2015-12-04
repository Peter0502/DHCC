#ifndef __lv_pldfr_CH__
#define __lv_pldfr_CH__
struct lv_pldfr_c{
	char      rowid[20];
	char      packid[9];
	char      orderno[9];
	long      wt_date;
	char      br_no[13];
	char      or_br_no[13];
	long      dsf_date;
	char      xyno[61];
	char      pay_br_no[13];
	char      pay_opn_br_no[13];
	char      pay_ac_no[33];
	char      pay_name[61];
	char      pay_addr[61];
	char      yw_type[6];
	double    totamt;
	char      cash_qs_no[13];
	long      cash_num;
	char      tx_mx_no[9];
	char      cash_br_no[13];
	char      cash_opn_br_no[13];
	char      cash_ac_no[33];
	char      cash_name[61];
	char      cash_addr[61];
	double    tx_amt;
	char      content[61];
	char      sts[3];
	char      beg_sts[3];
	char      operlevel[2];
	char      filename[257];
	char      repstat[3];
	double    succamt;
	long      succnum;
	char      re_content[61];
	long      re_dsf_date;
	char      repstat2[3];
	char      re_content2[61];
};
#endif 
