#ifndef __point_tx_def_CH__
#define __point_tx_def_CH__
struct point_tx_def_c{
	char      rowid[20];
	char      tx_code[5];
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
