#ifndef __lv_c552r_CH__
#define __lv_c552r_CH__
struct lv_c552r_c{
	char      rowid[20];
	char      cmtno[4];
	long      in_date;
	long      wt_date;
	char      brno[8];
	char      tlrno[7];
	char      sendco[5];
	char      acbrno[13];
	char      iftype[2];
	long      dtlid;
	char      cashno[13];
	char      paymon[7];
	long      sdate;
	long      edate;
	char      aorm[2];
	char      countno[13];
	char      totamt[16];
	char      ferate[7];
	char      acrate[7];
	char      porr[2];
	char      ccpcrate[7];
	char      totnomonth[9];
	char      monrate[7];
	char      totcrmonth[9];
	char      crrate[7];
	char      num[9];
	long      seqno;
	char      pkgno[4];
	char      sorr[2];
	char      iorw[2];
	char      stime[5];
	char      etime[5];
	char      totnum[9];
	char      brate[7];
	char      iorwrate[7];
	char      timerate[7];
	char      price[7];
	char      txamt[16];
};
#endif 
