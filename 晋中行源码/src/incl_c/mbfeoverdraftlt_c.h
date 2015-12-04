#ifndef __mbfeoverdraftlt_CH__
#define __mbfeoverdraftlt_CH__
struct mbfeoverdraftlt_c{
	char      rowid[20];
	long      row_id;
	long      consigndate;
	char      accrualtime[7];
	double    overdraftamnt;
	double    rate;
	double    accrualamnt;
};
#endif 
