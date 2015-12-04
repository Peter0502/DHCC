#ifndef __mbfeeisbank_CH__
#define __mbfeeisbank_CH__
struct mbfeeisbank_c{
	char      rowid[20];
	char      bnkno[7];
	char      eissiteno[5];
	char      cnapsflg[2];
	char      cnapsbnkno[13];
	char      bnksstat[2];
	char      bnkcity[5];
	char      bnklname[61];
	char      bnkssname[21];
	char      bnkaddr[61];
	char      bnkpostcode[7];
	char      bnktel[31];
	char      bkemail[31];
	char      bnkrmrk[61];
};
#endif 
