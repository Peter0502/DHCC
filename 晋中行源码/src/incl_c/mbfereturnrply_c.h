#ifndef __mbfereturnrply_CH__
#define __mbfereturnrply_CH__
struct mbfereturnrply_c{
	char      rowid[20];
	long      consigndate;
	long      origrtnappdate;
	char      origrtnappno[4];
	char      odficenter[5];
	char      isdficode[13];
	char      rtnrplybkcode[13];
	char      rdficenter[5];
	char      osdficode[13];
	char      rdficode[13];
	long      origconsigndate;
	char      origtxtype[2];
	char      origtxssno[9];
	double    origamount;
	char      rtnrplyuser[5];
	char      rtnrplyrus[2];
	char      procstate[3];
	char      statetime[15];
	char      errcode[9];
	char      errdesc[61];
	long      printno;
	char      rtnrplysource[3];
	char      strinfo[61];
};
#endif 
