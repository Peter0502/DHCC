#ifndef __auth_auth_lmt_CH__
#define __auth_auth_lmt_CH__
struct auth_auth_lmt_c{
	char      rowid[20];
	char      auth_code[4];
	double    min_amt;
	double    max_amt;
	char      mo[21];
};
#endif 
