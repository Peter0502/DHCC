#ifndef __in_mst_CH__
#define __in_mst_CH__
struct in_mst_c{
	char      rowid[20];
	char      opn_br_no[6];
	long      ac_id;
	long      ac_seqn;
	char      prdt_no[4];
	double    bal;
	double    ys_bal;
	double    hst_bal;
	char      sts[2];
	long      opn_date;
	long      ic_date;
	long      end_date;
	long      cls_date;
	long      lst_date;
	char      intst_knd[2];
	double    rate;
	double    over_rate;
	double    intst_acm;
	long      hst_cnt;
	long      hst_pg;
	double    hold_amt;
	char      od_ind[2];
	char      name[61];
	char      cal_code[11];
	char      mac[17];
};
#endif 
