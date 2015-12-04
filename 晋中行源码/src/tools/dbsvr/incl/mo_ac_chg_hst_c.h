#ifndef __mo_ac_chg_hst_CH__
#define __mo_ac_chg_hst_CH__
struct mo_ac_chg_hst_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      tx_code[5];
	char      tx_br_no[6];
	char      chg_tab_name[19];
	char      chg_fld_name[19];
	char      bef_chg_tent[41];
	char      aft_chg_tent[41];
	long      tx_date;
	long      tx_time;
	long      trace_no;
	long      trace_cnt;
	char      brf[21];
	char      tel[5];
	char      chk[5];
	char      auth[5];
};
#endif 
