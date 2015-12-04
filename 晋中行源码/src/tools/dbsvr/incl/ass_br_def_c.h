#ifndef __ass_br_def_CH__
#define __ass_br_def_CH__
struct ass_br_def_c{
	char      rowid[20];
	char      br_no[6];
	char      name[41];
	char      sub_br_no[6];
	long      beg_date;
	long      end_date;
	char      sts[2];
};
#endif 
