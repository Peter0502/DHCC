#ifndef __dc_branch_CH__
#define __dc_branch_CH__
struct dc_branch_c{
	char      rowid[20];
	char      br_no[6];
	char      br_name[21];
	char      cash_ac_ind[2];
	char      br_lvl[2];
	char      up_br_no[6];
};
#endif 
