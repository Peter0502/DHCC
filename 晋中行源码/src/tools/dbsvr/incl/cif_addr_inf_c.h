#ifndef __cif_addr_inf_CH__
#define __cif_addr_inf_CH__
struct cif_addr_inf_c{
	char      rowid[20];
	long      cif_no;
	long      addr_seqn;
	char      addr_type[3];
	char      country[4];
	char      addr[61];
	char      post_code[7];
};
#endif 
