#ifndef __point_prdt_def_CH__
#define __point_prdt_def_CH__
struct point_prdt_def_c{
	char      rowid[20];
	char      prdt_no[4];
	char      cif_type[2];
	long      beg_date;
	long      edn_date;
	double    beg_bal;
	double    edn_bal;
	long      max_point;
	long      min_point;
	long      ratio;
};
#endif 
