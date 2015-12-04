#ifndef __cif_crdt_inf_CH__
#define __cif_crdt_inf_CH__
struct cif_crdt_inf_c{
	char      rowid[20];
	char      credit_no[11];
	char      cur_no[3];
	double    bal;
	double    amt;
	char      cyc_ind[2];
};
#endif 
