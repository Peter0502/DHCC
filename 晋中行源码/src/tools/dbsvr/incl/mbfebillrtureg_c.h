#ifndef __mbfebillrtureg_CH__
#define __mbfebillrtureg_CH__
struct mbfebillrtureg_c{
	char      rowid[20];
	char      msssno[9];
	long      consigndate;
	char      odficode[13];
	char      rdficode[13];
	char      sts[2];
	char      payeropenaccbkcode[13];
	double    amount;
	long      billdate;
	char      billcode[9];
	char      billofxchngseal[11];
	char      appuser[5];
	char      billtype[2];
	char      origrdficode[13];
	char      sendtime[15];
	long      setdate;
	long      setflowno;
	char      errcode[9];
	char      errdesc[61];
	char      strinfo[61];
};
#endif 
