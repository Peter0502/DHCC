#ifndef __mbfeoprtfeecl_CH__
#define __mbfeoprtfeecl_CH__
struct mbfeoprtfeecl_c{
	char      rowid[20];
	long      consigndate;
	char      rdficenter[5];
	char      feebkcode[13];
	long      feebgdate;
	long      feeeddate;
	double    feeammunt;
	double    sapsrate;
	double    bktprate;
	double    mothrate;
	double    hvpfeeamt;
	long      hvpfeecnt;
	double    singlefeeamt;
	long      singlefeecnt;
	double    infofeeamt;
	long      infofeecnt;
	char      feeacno[20];
	long      printno;
};
#endif 
