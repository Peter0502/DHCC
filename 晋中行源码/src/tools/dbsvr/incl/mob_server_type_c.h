#ifndef __mob_server_type_CH__
#define __mob_server_type_CH__
struct mob_server_type_c{
	char      rowid[20];
	char      server_type[5];
	char      server_name[51];
	char      fee_term[2];
	double    fee_standard;
	char      server_sts[2];
};
#endif 
