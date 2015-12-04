#ifndef __bitmap_CH__
#define __bitmap_CH__
struct bitmap_c{
	char      rowid[20];
	long      bit_seq;
	char      bit_name[21];
	char      bit_type[2];
	char      fied_type[2];
	long      leng;
	char      flag[2];
};
#endif 
