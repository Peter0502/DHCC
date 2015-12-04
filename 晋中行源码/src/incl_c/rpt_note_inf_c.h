#ifndef __rpt_note_inf_CH__
#define __rpt_note_inf_CH__
struct rpt_note_inf_c{
	char      rowid[20];
	long      tx_date;
	long      trace_no;
	char      opn_br_no[10];
	char      tx_br_no[10];
	char      tx_opn_br_no[10];
	char      tx_tx_br_no[10];
	char      dc_ind[2];
	char      acc_hrt[8];
	char      tw_acchrt[8];
	char      ac_no[20];
	char      name[41];
	double    amt;
	char      cur_no[3];
	char      txcode[5];
	char      summary[41];
};
#endif 
