#ifndef __mo_sxc_CH__
#define __mo_sxc_CH__
struct mo_sxc_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	double    bal;
	double    intst;
	double    un_intst;
	long      tx_date;
	long      trace_no;
	long      untx_date;
	long      untrace_no;
	char      tx_br_no[6];
	char      tx_tel[5];
	char      untx_br_no[6];
	char      untx_tel[5];
	char      opn_br_no[6];
	char      sts[2];
	char      flag[2];
	char      qy_type[2];
};
#endif 
