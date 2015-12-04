#ifndef __mbfeoprtcancel_CH__
#define __mbfeoprtcancel_CH__
struct mbfeoprtcancel_c{
	char      rowid[20];
	char      cancelno[4];
	long      consigndate;
	char      applyuser[5];
	char      grantuser[5];
	long      origconsigndate;
	char      origodficode[13];
	char      cnapsodfi[13];
	char      origtxtype[2];
	char      origtxssno[9];
	char      cancelstrinfo[61];
	char      cancelresult[2];
	char      resultstrinfo[61];
	char      procstate[3];
	char      statetime[15];
};
#endif 
