#ifndef __global_CH__
#define __global_CH__
struct global_c{
	char      rowid[20];
	double    global_idx;
	char      global_region[8];
	char      global_tbsdy[9];
	char      global_bhdate[9];
	char      global_chdate[2];
	char      global_trace[2];
	char      global_trfile[31];
	char      global_rptpath[21];
	double    global_adtno;
	double    global_recno;
	double    global_rskey;
};
#endif 
