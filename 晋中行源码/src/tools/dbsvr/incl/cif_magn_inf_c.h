#ifndef __cif_magn_inf_CH__
#define __cif_magn_inf_CH__
struct cif_magn_inf_c{
	char      rowid[20];
	long      cif_no;
	char      units_license[21];
	char      ap_qualife[21];
	char      artificial_person[21];
	char      ap_id[21];
	char      credit_no[17];
	char      own_attri[2];
	char      ent_attri[2];
	char      credit[2];
	char      supervior[51];
	char      director[51];
	char      financer[51];
	char      opn_bank[51];
	double    opn_amt;
};
#endif 
