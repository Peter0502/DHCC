#ifndef __ln_mst_bk_CH__
#define __ln_mst_bk_CH__
struct ln_mst_bk_c{
	char      rowid[20];
	char      opn_br_no[6];
	char      prdt_no[4];
	double    bal;
	double    in_lo_intst;
	double    out_lo_intst;
	double    cmpd_lo_intst;
	double    gage_amt;
	long      cnt;
};
#endif 
