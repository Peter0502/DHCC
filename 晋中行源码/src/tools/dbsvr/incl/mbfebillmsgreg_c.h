#ifndef __mbfebillmsgreg_CH__
#define __mbfebillmsgreg_CH__
struct mbfebillmsgreg_c{
	char      rowid[20];
	char      msssno[9];
	char      sts[2];
	long      consigndate;
	char      rdficode[13];
	char      payeropenaccbkcode[13];
	char      payeracc[33];
	char      payername[61];
	char      payeraddr[61];
	long      billdate;
	char      billcode[9];
	char      billofxchngseal[11];
	double    remittanceamnt;
	double    repudiationamnt;
	char      rdficenter[5];
	char      recipientopenaccbk[13];
	char      lastbillholderacc[33];
	char      lastbillholdername[61];
	long      prmptpaydate;
	char      strinfo[61];
};
#endif 
