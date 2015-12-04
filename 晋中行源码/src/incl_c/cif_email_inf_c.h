#ifndef __cif_email_inf_CH__
#define __cif_email_inf_CH__
struct cif_email_inf_c{
	char      rowid[20];
	long      cif_no;
	long      addr_seqn;
	char      addr_type[3];
	char      email[21];
	char      link_man[61];
};
#endif 
