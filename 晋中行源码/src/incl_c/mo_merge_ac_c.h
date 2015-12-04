#ifndef __mo_merge_ac_CH__
#define __mo_merge_ac_CH__
struct mo_merge_ac_c{
	char      rowid[20];
	char      old_br_no[8];
	char      old_ac_no[21];
	char      br_no[6];
	char      ac_no[20];
	long      ac_id;
	long      ac_seqn;
	char      chg_mdm_yn[2];
};
#endif 
