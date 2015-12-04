#ifndef __cif_rel_chg_hst_CH__
#define __cif_rel_chg_hst_CH__
struct cif_rel_chg_hst_c{
	char      rowid[20];
	long      cif_no;
	char      chg_tablename[21];
	char      chg_columnname[21];
	long      chg_date;
	long      chg_time;
	char      bef_content[41];
	char      aft_content[41];
	char      clerk_no[6];
	char      rmk[41];
};
#endif 
