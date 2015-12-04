#ifndef __mbfenetbank_CH__
#define __mbfenetbank_CH__
struct mbfenetbank_c{
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
	char      content[61];
};
#endif 
