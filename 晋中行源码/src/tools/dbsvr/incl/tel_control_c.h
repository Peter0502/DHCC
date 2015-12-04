#ifndef __tel_control_CH__
#define __tel_control_CH__
struct tel_control_c{
	char      rowid[20];
	char      br_no[6];
	char      tel[5];
	long      tx_date;
	char      sts[2];
	char      msg[101];
};
#endif 
