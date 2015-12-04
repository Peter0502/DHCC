#ifndef __bzj_js_ac_rel_CH__
#define __bzj_js_ac_rel_CH__
struct bzj_js_ac_rel_c{
	char      rowid[20];
	char      tx_br_no[6];
	long      bzj_ac_id;
	long      bzj_ac_seqn;
	long      js_ac_id;
	long      js_ac_seqn;
	long      tx_date;
	long      trace_no;
	char      tel[5];
	char      sts[2];
	char      filler[61];
};
#endif 
