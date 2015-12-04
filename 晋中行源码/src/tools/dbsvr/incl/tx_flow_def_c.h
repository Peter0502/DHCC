#ifndef __tx_flow_def_CH__
#define __tx_flow_def_CH__
struct tx_flow_def_c{
	char      rowid[20];
	char      tx_code[5];
	long      flow_code;
	char      sub_tx_code[5];
	char      sub_tx_rslt[201];
};
#endif 
