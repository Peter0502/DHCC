#ifndef __lv_pldsfqugx_CH__
#define __lv_pldsfqugx_CH__
struct lv_pldsfqugx_c{
	char      rowid[20];
	char      pkgno[4];
	char      orderno[9];
	long      wt_date;
	char      br_no[13];
	char      or_br_no[13];
	char      packid[9];
	char      xyno[61];
	char      xy_ind[2];
	char      ch_ind[2];
	char      pay_br_no[13];
	char      pay_qs_no[13];
	char      pay_opn_br_no[13];
	char      pay_ac_no[33];
	char      pay_name[61];
	char      pay_addr[61];
	char      content[61];
	char      sts[3];
	char      beg_sts[3];
	char      operlevel[2];
	long      re_dsf_date;
	char      repstat[3];
	char      re_content2[61];
};
#endif 
