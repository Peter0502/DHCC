#ifndef __chnl_tradelimit_CH__
#define __chnl_tradelimit_CH__
struct chnl_tradelimit_c{
	char      rowid[20];
	char      chnl[2];
	long      pwd_cnt;
	long      wd_cnt;
	double    wd_amt;
	double    wd_ttl_max_amt;
};
#endif 
