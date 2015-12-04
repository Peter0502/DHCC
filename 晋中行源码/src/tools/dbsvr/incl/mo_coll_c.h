#ifndef __mo_coll_CH__
#define __mo_coll_CH__
struct mo_coll_c{
	char      rowid[20];
	char      ac_no[20];
	char      name[61];
	long      pick_date;
	char      pick_br_no[6];
	char      pick_tel[5];
	long      ret_date;
	char      ret_br_no[6];
	char      sts[2];
	long      wrk_date;
	long      trance_no;
	char      ret_tel[5];
};
#endif 
