#ifndef __cif_cif_rel_CH__
#define __cif_cif_rel_CH__
struct cif_cif_rel_c{
	char      rowid[20];
	long      cif_no;
	long      rel_cid_no;
	char      rel_code[4];
	char      rel_sts[2];
	long      crt_date;
	long      cls_date;
};
#endif 
