#ifndef __com_tel_change_CH__
#define __com_tel_change_CH__
struct com_tel_change_c{
	char      rowid[20];
	char      tel[5];
	char      br_no[6];
	char      name[21];
	char      pwd[7];
	char      lvl[2];
	char      to_tel[5];
	char      to_lvl[2];
	char      sts[2];
	char      filler[61];
};
#endif 
