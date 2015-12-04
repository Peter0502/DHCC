#ifndef __mo_qzgl_hst_CH__
#define __mo_qzgl_hst_CH__
struct mo_qzgl_hst_c{
	char      rowid[20];
	long      child_ac_id;
	long      part_ac_id;
	char      tx_br_no[61];
	long      trace_no;
	long      tx_date;
	double    tx_amt;
	char      tx_tel[5];
	double    child_bal;
};
#endif 
