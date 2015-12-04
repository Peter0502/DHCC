#ifndef __sh_com_branch_CH__
#define __sh_com_branch_CH__
struct sh_com_branch_c{
	char      rowid[20];
	char      br_no[6];
	char      br_name[31];
	char      wrk_sts[11];
	char      br_type[2];
	char      ipaddr[16];
	char      ind[2];
	long      city_no;
	char      spe_ind[11];
	char      up_br_no[6];
	long      tx_type;
	char      br_tele[21];
};
#endif 
