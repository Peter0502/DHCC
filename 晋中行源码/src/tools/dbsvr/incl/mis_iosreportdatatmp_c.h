#ifndef __mis_iosreportdatatmp_CH__
#define __mis_iosreportdatatmp_CH__
struct mis_iosreportdatatmp_c{
	char      rowid[20];
	char      brno[8];
	char      pigday[9];
	long      flag;
	long      reprownum;
	char      name[61];
	char      tdbal[19];
	char      ydbal[19];
	double    tmbal;
	double    tdmbal;
	double    tcmbal;
	long      cdcnt;
	double    cdamt;
	long      cccnt;
	double    ccamt;
	long      mdcnt;
	double    mdamt;
	long      mccnt;
	double    mcamt;
	char      fillter[11];
};
#endif 
