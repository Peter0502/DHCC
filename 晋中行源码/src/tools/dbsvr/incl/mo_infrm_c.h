#ifndef __mo_infrm_CH__
#define __mo_infrm_CH__
struct mo_infrm_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      infrm_date;
	long      prm_draw_date;
	double    prm_draw_amt;
	double    fct_wd_amt;
	long      occ_date;
	char      sts[2];
	char      name[61];
	char      phone[21];
	long      wrk_date;
	long      trace_no;
	char      id_knd[2];
	char      id_no[21];
};
#endif 
