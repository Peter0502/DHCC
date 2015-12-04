#ifndef __cif_crdt_use_det_CH__
#define __cif_crdt_use_det_CH__
struct cif_crdt_use_det_c{
	char      rowid[20];
	long      cif_no;
	char      credit_no[11];
	long      wrkdate;
	long      trace_no;
	double    amt;
	char      memo[41];
	char      flag[2];
};
#endif 
