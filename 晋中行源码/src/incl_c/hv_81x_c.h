#ifndef __hv_81x_CH__
#define __hv_81x_CH__
struct hv_81x_c{
	char      rowid[20];
	char      cmtno[4];
	char      br_no[8];
	char      in_date[9];
	char      txtime[9];
	long      orderno;
	long      dtlid;
	char      chgtype[2];
	char      receco[5];
	char      cash_qs_no[13];
	char      dr_br_no[13];
	char      totmon[7];
	char      sdate[9];
	char      edate[9];
	double    tot_amt;
	double    txamt;
	char      num[6];
	char      csbrno[13];
	double    amt;
	double    zf_amt;
	double    xx_amt;
};
#endif 
