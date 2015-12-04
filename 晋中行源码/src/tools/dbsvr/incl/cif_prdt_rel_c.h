#ifndef __cif_prdt_rel_CH__
#define __cif_prdt_rel_CH__
struct cif_prdt_rel_c{
	char      rowid[20];
	long      cif_no;
	char      prdt_code[4];
	long      ac_id;
	char      relation_no[3];
	char      relation_flag[2];
	long      addr_seri;
	long      crt_date;
	long      cls_date;
};
#endif 
