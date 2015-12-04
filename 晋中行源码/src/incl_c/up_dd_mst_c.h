#ifndef __up_dd_mst_CH__
#define __up_dd_mst_CH__
struct up_dd_mst_c{
	char      rowid[20];
	char      br_no[6];
	char      ac_no[20];
	double    bal;
	double    ys_bal;
	double    intst_acm;
	long      opn_date;
	long      ic_date;
	long      lst_date;
	long      hst_cnt;
	long      hst_pg;
	char      name[51];
	char      ac_sts[2];
};
#endif 
