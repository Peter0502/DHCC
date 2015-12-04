#ifndef __cif_blklst_rel_CH__
#define __cif_blklst_rel_CH__
struct cif_blklst_rel_c{
	char      rowid[20];
	long      cif_no;
	char      blalist_origin[2];
	char      relation_sts[2];
	char      rmk[41];
	long      crt_date;
	long      cls_date;
};
#endif 
