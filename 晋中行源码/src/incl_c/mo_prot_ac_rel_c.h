#ifndef __mo_prot_ac_rel_CH__
#define __mo_prot_ac_rel_CH__
struct mo_prot_ac_rel_c{
	char      rowid[20];
	char      tx_br_no[6];
	char      prot_type[4];
	long      main_ac_id;
	long      main_ac_seqn;
	long      app_ac_id;
	long      app_ac_seqn;
	long      tx_date;
	long      trace_no;
	char      tel[5];
	char      sts[2];
};
#endif 
