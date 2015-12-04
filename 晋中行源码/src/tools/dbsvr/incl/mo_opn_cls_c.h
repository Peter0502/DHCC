#ifndef __mo_opn_cls_CH__
#define __mo_opn_cls_CH__
struct mo_opn_cls_c{
	char      rowid[20];
	char      ac_no[20];
	long      ac_id;
	long      ac_seqn;
	char      cur_no[3];
	char      name[61];
	char      prdt_no[4];
	char      acc_hrt[6];
	char      cif_type[2];
	double    amt;
	char      opn_br_no[6];
	long      tx_date;
	char      tx_brno[6];
	long      trace_no;
	char      tel[5];
	char      chk[5];
	char      auth[5];
	char      check_no[21];
	char      ct_ind[2];
	char      wrk_sts[2];
};
#endif 
