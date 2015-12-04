#ifndef __lv_pldscx_CH__
#define __lv_pldscx_CH__
struct lv_pldscx_c{
	char      rowid[20];
	char      packid[9];
	char      orderno[9];
	long      wt_date;
	char      br_no[13];
	char      or_br_no[13];
	long      o_wt_date;
	char      o_orderno[9];
	char      o_yw_type[6];
	long      dsf_date;
	char      content[61];
	char      sts[3];
	char      beg_sts[3];
	char      operlevel[2];
	long      re_dsf_date;
	char      repstat2[3];
	char      re_content2[61];
};
#endif 
