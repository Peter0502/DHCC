#ifndef __lv_wbctl_CH__
#define __lv_wbctl_CH__
struct lv_wbctl_c{
	char      rowid[20];
	long      pack_date;
	char      packid[9];
	char      pkgno[4];
	char      pay_qs_no[13];
	char      cash_qs_no[13];
	long      in_date;
	char      packmy[41];
	long      dtlcnt;
	double    totamt;
	char      zcnode[2];
	long      zc_date;
	char      zcid[3];
	long      hz_date;
	long      o_pack_date;
	char      o_packid[9];
	long      dtlsuccnt;
	double    dtlsucamt;
	char      o_pkgno[4];
	char      ornode[13];
	char      acnode[13];
	char      ffflag[2];
	char      txnum[6];
	char      bfflag[2];
	long      qs_date;
	char      packstat[3];
	char      stat[2];
	char      brprtflag[65];
	char      qsprtflag[65];
	char      content[65];
	char      br_no[8];
	char      filler[61];
};
#endif 
