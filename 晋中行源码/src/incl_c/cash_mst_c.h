#ifndef __cash_mst_CH__
#define __cash_mst_CH__
struct cash_mst_c{
	char      rowid[20];
	char      br_no[6];
	char      tel[5];
	char      cur_no[3];
	char      ac_no[11];
	double    bal;
	double    ys_bal;
	long      t10000;
	long      t5000;
	long      t2000;
	long      t1000;
	long      t500;
	long      t200;
	long      t100;
	long      t50;
	long      t20;
	long      t10;
	long      t5;
	long      t2;
	long      t1;
	char      sts[2];
};
#endif 
