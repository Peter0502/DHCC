#ifndef __intst_hst_CH__
#define __intst_hst_CH__
struct intst_hst_c{
	char      rowid[20];
	char      br_no[6];
	long      ac_id;
	long      ac_seqn;
	char      prdt_no[4];
	char      cur_no[3];
	double    bic_bal;
	double    bic_acm;
	double    fct_intst;
	double    intst_tax;
	double    aic_bal;
	double    aic_acm;
	long      ic_date;
	long      intst_date;
	char      ac_ind[2];
	char      pt_ind[2];
	double    rate;
};
#endif 
