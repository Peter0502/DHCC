#ifndef __agent_info_CH__
#define __agent_info_CH__
struct agent_info_c{
	char      rowid[20];
	char      ac_no[20];
	char      type[2];
	long      tx_date;
	char      br_no[7];
	char      tel[5];
	char      agent_name[61];
	char      id_type[2];
	char      id_no[31];
	char      phone[21];
};
#endif 
