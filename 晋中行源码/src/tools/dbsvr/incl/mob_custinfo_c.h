#ifndef __mob_custinfo_CH__
#define __mob_custinfo_CH__
struct mob_custinfo_c{
	char      rowid[20];
	char      cust_no[11];
	char      cust_name[51];
	char      mobile[16];
	long      group;
	char      passwd[7];
};
#endif 
