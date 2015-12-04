#ifndef __ass_cp_stat_CH__
#define __ass_cp_stat_CH__
struct ass_cp_stat_c{
	char      rowid[20];
	char      gsp_no[5];
	long      date;
	char      br_no[6];
	char      prdt_code[4];
	double    opn_ac_amt;
	double    cls_ac_amt;
	long      opn_ac_cnt;
	long      cls_ac_cnt;
	double    ttl_amt;
	long      ttl_cnt;
};
#endif 
