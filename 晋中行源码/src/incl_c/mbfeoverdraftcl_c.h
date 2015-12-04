#ifndef __mbfeoverdraftcl_CH__
#define __mbfeoverdraftcl_CH__
struct mbfeoverdraftcl_c{
	char      rowid[20];
	long      consigndate;
	char      rdficenter[5];
	char      osdficode[13];
	char      rdficode[13];
	double    accrualamnt;
	char      accruacno[20];
	long      printno;
};
#endif 
