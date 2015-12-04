#ifndef __data_dic_CH__
#define __data_dic_CH__
struct data_dic_c{
	char      rowid[20];
	char      data_code[5];
	char      data_type[2];
	long      data_leng;
	long      data_dec;
	char      data_name[21];
	long      bit_seq;
};
#endif 
