#ifndef __mbfeghvsendjrn_CH__
#define __mbfeghvsendjrn_CH__
struct mbfeghvsendjrn_c{
	char      rowid[20];
	char      tx_br_no[6];
	char      tx_code[5];
	long      consigndate;
	long      trace_no;
	char      procstate[2];
	char      payeracc[33];
	double    amount;
	char      payername[61];
};
#endif 
