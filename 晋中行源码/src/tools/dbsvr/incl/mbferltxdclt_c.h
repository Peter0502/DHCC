#ifndef __mbferltxdclt_CH__
#define __mbferltxdclt_CH__
struct mbferltxdclt_c{
	char      rowid[20];
	long      consigndate;
	char      bankcode[13];
	long      txsbatno;
	char      oprttype[4];
	char      txssno[9];
	double    amount;
	char      osdficodeccpc[5];
	char      osdficode[13];
	char      payeracc[33];
	char      payername[61];
	char      payerbnk[13];
	char      isdficodeccpc[5];
	char      isdficode[13];
	char      recipientacc[33];
	char      recipientname[61];
	char      recipientbnk[13];
	char      bondcode[13];
	double    bondamount;
	double    setamount;
	long      setdate;
	double    setrate;
	char      bondordercode[10];
	double    netamount;
	double    bondrate;
	char      procstate[3];
	char      checkstate[3];
	char      statetime[15];
	long      printno;
	long      flowno;
	char      errcode[9];
	char      errdesc[61];
	char      oprtsource[3];
	char      strinfo[61];
};
#endif 
