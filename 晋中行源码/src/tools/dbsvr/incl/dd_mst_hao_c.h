#ifndef __dd_mst_hao_CH__
#define __dd_mst_hao_CH__
struct dd_mst_hao_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      opn_br_no[6];
	char      prdt_no[4];
	double    bal;
	double    acbo_bal;
	double    ys_bal;
	double    hst_bal;
	double    rate;
	double    flt_ratio;
	double    intst_acm;
	char      cash_ind[2];
	char      sttl_type[2];
	char      ac_type[2];
	char      intst_type[2];
	long      hst_cnt;
	long      hst_pg;
	long      opn_date;
	long      ic_date;
	long      lst_date;
	long      val_date;
	char      odtt_ind[2];
	char      ac_sts[2];
	char      hold_sts[2];
	double    hold_amt;
	double    ctl_amt;
	char      od_ind[2];
	char      draw_pwd[2];
	long      cif_no;
	char      name[51];
	char      cal_code[11];
	char      mac[17];
};
#endif 
