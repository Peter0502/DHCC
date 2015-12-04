#ifndef __seltelreg_CH__
#define __seltelreg_CH__
struct seltelreg_c{
	char      rowid[20];
	char      qbrname[51];
	char      qdate[9];
	char      qmode[2];
	char      qvocnum[17];
	double    telamt;
	char      rdate[9];
	char      rmode[2];
	char      rtxt[257];
	char      cxnr[257];
	long      tx_date;
	char      memo3[51];
	char      memo4[51];
};
#endif 
