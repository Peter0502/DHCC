#ifndef __cif_wrk_hst_inf_CH__
#define __cif_wrk_hst_inf_CH__
struct cif_wrk_hst_inf_c{
	char      rowid[20];
	long      cif_no;
	long      wrk_no;
	char      industry_no[4];
	char      vocation_no[4];
	char      position_no[4];
	long      beg_date;
	char      wrk_ind[2];
	long      end_date;
	char      unit_name[51];
	double    income;
};
#endif 
