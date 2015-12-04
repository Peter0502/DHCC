#ifndef __com_rollback_ctl_CH__
#define __com_rollback_ctl_CH__
struct com_rollback_ctl_c{
	char      rowid[20];
	char      sub_tx_code[5];
	char      reg_ind[201];
	char      memo[11];
};
#endif 
