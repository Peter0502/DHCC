#ifndef __td_parm_CH__
#define __td_parm_CH__
struct td_parm_c{
	char      rowid[20];
	char      prdt_no[4];
	char      title[31];
	long      beg_date;
	long      end_date;
	long      term;
	char      term_type[2];
	char      cif_type[2];
	char      cur_no[3];
	char      bal_type[2];
	double    min_bal;
	double    max_bal;
	double    min_opn_amt;
	double    max_opn_amt;
	char      td_type[2];
	char      rate_no[4];
	char      rate_type[2];
	char      rate_mode[2];
	char      rate_func_code[4];
	double    min_flt_ratio;
	double    max_flt_ratio;
	char      intst_term_type[2];
	char      keep_ind[2];
	char      ovrd_intst_ind[2];
	char      ovrd_rate_no[4];
	char      felbck_rate_no[4];
	char      intst_type[2];
	char      acm_calc_mode[2];
	long      intst_mon;
	long      intst_day;
	char      tfr_lmt_ind[2];
	char      tfr_prt_ind[2];
	char      atterm_prt_ind[2];
	char      atterm_date_ind[2];
	char      intst_tax_no[4];
	char      cash_ind[2];
	char      dpst_ind[2];
	double    dpst_min_amt;
	double    draw_min_amt;
	char      wdr_ind[2];
	char      fwd_draw_ind[2];
	long      wdr_lmt_cnt;
	char      thr_dpst_ind[2];
	char      thr_draw_ind[2];
	char      thr_opn_ind[2];
	char      thr_cls_ind[2];
	long      dedr_intvl;
	char      intvl_type[2];
	char      edu_pro_ind[2];
	char      holi_ind[2];
	long      opn_lmt;
	long      cls_lmt;
	char      dc_code[5];
	char      put_ind[2];
	char      br_no[6];
	long      prt_no;
};
#endif 
