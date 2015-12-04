#ifndef __ass_mst_CH__
#define __ass_mst_CH__
struct ass_mst_c{
	char      rowid[20];
	char      br_no[6];
	long      pact_no;
	char      sub_br_no[6];
	char      sign[2];
	double    bal;
	double    ys_bal;
	long      ic_date;
	long      end_date;
	long      cls_date;
	long      lst_date;
	double    rate;
	double    over_rate;
	double    intst;
	double    intst_acm;
	long      hst_cnt;
	char      brf[61];
	char      sts[2];
};
#endif 
