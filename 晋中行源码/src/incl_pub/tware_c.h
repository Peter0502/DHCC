#ifndef __tware_CH__
#define __tware_CH__
struct tware_c{
	char      rowid[20];
	char      br_no[6];
	char      tel[5];
	long      trace_no;
	long      tx_date;
	long      tx_time;
	double    amt;
	double    bal;
	char      stat[2];
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
};
#endif 
