#ifndef __mbfeoprtfeelt_CH__
#define __mbfeoprtfeelt_CH__
struct mbfeoprtfeelt_c{
	char      rowid[20];
	long      row_id;
	long      consigndate;
	char      paytools[2];
	char      cmtno[4];
	char      srflag[2];
	char      feeptmlow[21];
	char      feeptmupp[21];
	char      feecnt[11];
	double    feeprice;
	double    feerate;
	double    timespec;
	double    feeamnt;
};
#endif 
