#ifndef __ass_subs_intst_CH__
#define __ass_subs_intst_CH__
struct ass_subs_intst_c{
	char      rowid[20];
	char      brno[6];
	long      date;
	long      beg_date;
	long      end_date;
	double    acm;
	double    rate;
	char      deal_ind[2];
	char      dc_ind[2];
};
#endif 
