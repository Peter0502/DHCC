#ifndef __ass_tel_pflio_stat_CH__
#define __ass_tel_pflio_stat_CH__
struct ass_tel_pflio_stat_c{
	char      rowid[20];
	char      br_no[6];
	char      tel[6];
	char      prdt_code[4];
	long      date;
	char      trad_code[5];
	char      ct_ind[2];
	long      cnt;
	double    amt;
};
#endif 
