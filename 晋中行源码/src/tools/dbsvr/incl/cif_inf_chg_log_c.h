#ifndef __cif_inf_chg_log_CH__
#define __cif_inf_chg_log_CH__
struct cif_inf_chg_log_c{
	char      rowid[20];
	long      cif_no;
	char      chg_tablename[21];
	char      chg_columnname[21];
	long      chg_date;
	long      chg_time;
	char      bef_content[101];
	char      aft_content[101];
	char      clerk_no[6];
	char      memo[41];
};
#endif 
