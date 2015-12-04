#ifndef __com_tel_connect_CH__
#define __com_tel_connect_CH__
struct com_tel_connect_c{
	char      rowid[20];
	char      tel[5];
	char      tw_tel[5];
	double    amt;
	char      br_no[6];
	long      tx_date;
	char      cur_no[3];
	long      trace_no;
	char      ind[2];
};
#endif 
