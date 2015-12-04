#ifndef __cif_basic_inf_CH__
#define __cif_basic_inf_CH__
struct cif_basic_inf_c{
	char      rowid[20];
	long      cif_no;
	char      type[2];
	char      name[61];
	char      local[2];
	char      poten[2];
	long      crt_date;
	long      crt_time;
	char      lvl[2];
	char      shareholder_ind[2];
	long      cif_seqn;
	char      sts[2];
};
#endif 
