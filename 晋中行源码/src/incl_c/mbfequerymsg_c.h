#ifndef __mbfequerymsg_CH__
#define __mbfequerymsg_CH__
struct mbfequerymsg_c{
	char      rowid[20];
	char      querymsgno[9];
	long      querydate;
	char      odficenter[5];
	char      isdficode[13];
	char      querybkcode[13];
	char      rdficenter[5];
	char      osdficode[13];
	char      rplybkcode[13];
	long      origconsigndate;
	char      origodficode[13];
	char      origrdficode[13];
	char      origtxtype[2];
	char      origtxssno[9];
	double    origamount;
	char      querycontent[256];
	char      queryuser[5];
	char      procstate[3];
	char      statetime[15];
	char      errcode[9];
	char      errdesc[61];
	long      printno;
	char      querysource[3];
};
#endif 
