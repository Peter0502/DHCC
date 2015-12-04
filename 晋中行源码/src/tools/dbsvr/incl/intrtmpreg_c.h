#ifndef __intrtmpreg_CH__
#define __intrtmpreg_CH__
struct intrtmpreg_c{
	char      rowid[20];
	char      actno[19];
	long      actnoseq;
	char      kinbr[8];
	char      txdate[9];
	long      txseq;
	char      atiday[9];
	char      adjday[9];
	double    rate;
	double    dinum;
	double    intr;
	long      intr_type;
	double    avbal;
	char      stat[2];
};
#endif 
