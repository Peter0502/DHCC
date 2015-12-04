#ifndef __auth_code_CH__
#define __auth_code_CH__
struct auth_code_c{
	char      rowid[20];
	char      br_no[6];
	long      auth_no;
	long      trace_no;
	long      done_trace_no;
	char      tx_code[5];
	long      cond_seqn;
	char      lvl[11];
	char      auth_ind[2];
	char      sts[2];
	char      tel[5];
	char      auth[5];
	char      aud[5];
	char      chk[5];
};
#endif 
