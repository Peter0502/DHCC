#ifndef __cms_rollback_CH__
#define __cms_rollback_CH__
struct cms_rollback_c{
	char      rowid[20];
	char      cms_sq_no[15];
	char      do_type[2];
	char      beg_table[21];
	char      beg_sql[201];
	char      rolbk_sql[2001];
	char      sts[2];
	char      filler[1001];
};
#endif 
