#ifndef __auth_flow_CH__
#define __auth_flow_CH__
struct auth_flow_c{
	char      rowid[20];
	char      flow_code[4];
	long      seqn;
	char      act_code[4];
	char      spe_ind[11];
};
#endif 
