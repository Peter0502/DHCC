#ifndef __hv_chargemx_CH__
#define __hv_chargemx_CH__
struct hv_chargemx_c{
	char      rowid[20];
	char      br_no[8];
	long      in_date;
	long      dtlid;
	char      chgtype[2];
	char      cmtno[4];
	char      srflag[2];
	char      addid[9];
	long      sdate;
	long      edate;
	char      lowlimit[7];
	char      upplimit[7];
	char      mnum[11];
	char      price[7];
	char      brate[7];
	char      pecrate[7];
	char      txamt[16];
	char      prtflag[2];
};
#endif 
