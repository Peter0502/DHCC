#ifndef __data_dictmp_CH__
#define __data_dictmp_CH__
struct data_dictmp_c{
	char      rowid[20];
	char      data_code[5];
	char      data_type[2];
	long      data_leng;
	long      data_dec;
	char      data_name[21];
	long      bit_seq;
};
#endif 
