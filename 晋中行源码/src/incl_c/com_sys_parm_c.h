#ifndef __com_sys_parm_CH__
#define __com_sys_parm_CH__
struct com_sys_parm_c{
	char      rowid[20];
	long      sys_sts;
	long      lst_date;
	long      sys_date;
	char      real_time_ind[2];
	char      sys_ind[11];
};
#endif 
