#ifndef __mo_qzgl_CH__
#define __mo_qzgl_CH__
struct mo_qzgl_c{
	char      rowid[20];
	long      child_ac_id;
	long      part_ac_id;
	long      tx_date;
	char      tx_br_no[6];
	long      trace_no;
	char      tx_tel[5];
	long      untx_date;
	char      untx_br_no[6];
	long      untrace_no;
	char      untx_tel[5];
	double    qy_amt;
	char      sts[2];
	long      hz_date;
};
#endif 
