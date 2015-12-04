#ifndef __tlrctl_CH__
#define __tlrctl_CH__
struct tlrctl_c{
	char      rowid[20];
	char      brno[6];
	char      type[2];
	char      tlrno[5];
	char      passwd[7];
	char      name[21];
	char      trmno[3];
	char      txdate[9];
	long      ejfno;
	long      wrgnum;
	char      edate[9];
	char      auth[9];
};
#endif 
