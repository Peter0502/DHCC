#ifndef __atmrecord_CH__
#define __atmrecord_CH__
struct atmrecord_c{
	char      rowid[20];
	char      atmno[11];
	char      atmid[21];
	char      atmmode[2];
	char      opnbr[8];
	char      depflag[2];
	char      chaflag[2];
	char      userflag[2];
	char      atmstat[2];
	long      paycnt;
	double    payamt;
	double    paymamt;
	double    feerate;
	char      opndate[9];
	char      startdte[9];
	char      enddate[9];
	char      place[41];
	char      tlrno[7];
	char      filler[61];
};
#endif 
