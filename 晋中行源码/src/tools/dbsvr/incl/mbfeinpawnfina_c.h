#ifndef __mbfeinpawnfina_CH__
#define __mbfeinpawnfina_CH__
struct mbfeinpawnfina_c{
	char      rowid[20];
	char      txssno[9];
	long      consigndate;
	char      spjoincode[13];
	char      osdficodeccpc[5];
	char      osdficode[13];
	char      isdficodeccpc[5];
	char      isdficode[13];
	double    amount;
	double    finaamt;
	double    finarest;
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
