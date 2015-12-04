#ifndef __mob_recvmail_CH__
#define __mob_recvmail_CH__
struct mob_recvmail_c{
	char      rowid[20];
	long      tx_date;
	long      tx_time;
	char      opt_code[4];
	char      mobile[16];
	char      message[141];
	char      proc_flag[2];
};
#endif 
