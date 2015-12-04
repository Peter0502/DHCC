#ifndef __com_eod_exec_CH__
#define __com_eod_exec_CH__
struct com_eod_exec_c{
	char      rowid[20];
	long      do_seq;
	char      sub_tx_code[5];
	char      prog[31];
	char      prog_name[21];
	char      br_type[11];
	char      exec_pd[2];
	char      pd_month[3];
	char      exec_beg[3];
	char      exec_end[3];
	char      do_type[2];
	char      using[2];
	char      holiday[2];
	char      finish[2];
	char      eoy_num[2];
	char      eoy_finish[2];
};
#endif 
