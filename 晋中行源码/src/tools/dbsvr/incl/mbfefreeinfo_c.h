#ifndef __mbfefreeinfo_CH__
#define __mbfefreeinfo_CH__
struct mbfefreeinfo_c{
	char      rowid[20];
	char      msssno[9];
	long      consigndate;
	char      odficenter[5];
	char      isdficode[13];
	char      odficode[13];
	char      rdficenter[5];
	char      osdficode[13];
	char      rdficode[13];
	char      procstate[3];
	char      statetime[15];
	char      errcode[9];
	char      errdesc[61];
	char      putuser[5];
	char      infosource[3];
	char      infomation[256];
	long      printno;
};
#endif 
