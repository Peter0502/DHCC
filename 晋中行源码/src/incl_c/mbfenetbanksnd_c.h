#ifndef __mbfenetbanksnd_CH__
#define __mbfenetbanksnd_CH__
struct mbfenetbanksnd_c{
	char      rowid[20];
	char      nbkcode[13];
	char      sabkcode[13];
	char      bnkcity[5];
	char      nbkname[61];
	char      nbksname[21];
	char      nbkaddrss[61];
	char      cnttel[21];
	char      cntper[17];
	char      postcode[7];
	char      nbkstate[2];
	char      bkemail[31];
	char      opertype[2];
	long      opereffdate;
	char      content[61];
};
#endif 
