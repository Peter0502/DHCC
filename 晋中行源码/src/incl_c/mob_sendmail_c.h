#ifndef __mob_sendmail_CH__
#define __mob_sendmail_CH__
struct mob_sendmail_c{
	char      rowid[20];
	char      tx_code[5];
	long      ac_id;
	char      ac_no[21];
	char      tel[7];
	long      trace_no;
	long      tx_date;
	long      tx_time;
	char      opt_no[4];
	char      mobile[51];
	char      message[141];
	char      timeflag[2];
	long      t_date;
	long      t_time;
	char      send_level[2];
	long      sene_times;
	long      send_date;
	long      send_time;
	double    sxf_amt;
	char      proc_flag[2];
	double    tt_no;
	char      tx_type[2];
	double    mobseq;
};
#endif 
