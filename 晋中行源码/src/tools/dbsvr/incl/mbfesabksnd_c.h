#ifndef __mbfesabksnd_CH__
#define __mbfesabksnd_CH__
struct mbfesabksnd_c{
	char      rowid[20];
	char      sabkcode[13];
	char      sabkname[61];
	char      sabksname[21];
	char      sabkclass[3];
	char      sabkstat[2];
	char      sbksupr[71];
	char      blccpc[5];
	char      bnkcity[5];
	char      sabkaddress[61];
	char      postcode[7];
	char      cnttel[21];
	char      bkemail[31];
	char      opertype[2];
	long      opereffdate;
	char      content[61];
};
#endif 
