#ifndef __bplosthis_CH__
#define __bplosthis_CH__
struct bplosthis_c{
	char      rowid[20];
	long      lostday;
	char      pono[21];
	long      lostcnt;
	long      signday;
	long      unlostday;
	char      lostbrno[8];
	char      unlostbrno[8];
	char      inflag[2];
	char      bptype[2];
	char      paybrno[13];
	char      paybrname[61];
	char      cashbrno[13];
	char      cashbrname[61];
	double    avbal;
	char      feeflag[2];
	double    feeamt;
	char      stat[2];
	char      losttlrno[7];
	char      unlosttlrno[7];
	char      filler[61];
};
#endif 
