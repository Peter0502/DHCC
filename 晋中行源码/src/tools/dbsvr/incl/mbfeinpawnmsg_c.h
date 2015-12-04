#ifndef __mbfeinpawnmsg_CH__
#define __mbfeinpawnmsg_CH__
struct mbfeinpawnmsg_c{
	char      rowid[20];
	long      consigndate;
	char      spjoincode[13];
	char      txssno[9];
	double    amount;
	char      osdficodeccpc[5];
	char      osdficode[13];
	char      isdficodeccpc[5];
	char      isdficode[13];
	char      procstate[3];
	char      checkstate[3];
	char      statetime[15];
	long      printno;
	long      flowno;
	char      errcode[9];
	char      errdesc[61];
	char      strinfo[61];
};
#endif 
