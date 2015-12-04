#ifndef __mbfereturnapp_CH__
#define __mbfereturnapp_CH__
struct mbfereturnapp_c{
	char      rowid[20];
	char      rtnappno[4];
	long      consigndate;
	char      odficenter[5];
	char      isdficode[13];
	char      odficode[13];
	char      rdficenter[5];
	char      osdficode[13];
	char      rdficode[13];
	long      origconsigndate;
	char      origtxtype[2];
	char      origtxssno[9];
	double    origamount;
	char      appuser[5];
	char      procstate[3];
	char      statetime[15];
	char      errcode[9];
	char      errdesc[61];
	long      printno;
	char      rtnappsource[3];
	char      strinfo[61];
};
#endif 
