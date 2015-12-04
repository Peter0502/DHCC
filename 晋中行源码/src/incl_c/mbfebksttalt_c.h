#ifndef __mbfebksttalt_CH__
#define __mbfebksttalt_CH__
struct mbfebksttalt_c{
	char      rowid[20];
	long      consigndate;
	char      txssno[9];
	double    amount;
	char      srflag[2];
	char      odficode[13];
	char      rdficode[13];
	char      oprttype[3];
	char      warrantno[9];
	double    compensationamnt;
	char      tel[5];
	long      tx_date;
	long      tx_time;
	char      strinfo[61];
};
#endif 
