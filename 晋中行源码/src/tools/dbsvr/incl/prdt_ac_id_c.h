#ifndef __prdt_ac_id_CH__
#define __prdt_ac_id_CH__
struct prdt_ac_id_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	char      ac_id_type[2];
	char      prdt_no[4];
	long      cif_no;
};
#endif 
