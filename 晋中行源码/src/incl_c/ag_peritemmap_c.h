#ifndef __ag_peritemmap_CH__
#define __ag_peritemmap_CH__
struct ag_peritemmap_c{
	char      rowid[20];
	char      unit_no[5];
	char      item_no[21];
	char      name[51];
	char      id_type[2];
	char      id_no[21];
	char      ac_no[20];
	char      remark[51];
	long      bgn_use_date;
	char      bat_flag[2];
	char      stat[2];
	long      opn_date;
	char      opn_tel[5];
	long      cls_date;
	char      cls_tel[5];
};
#endif 
