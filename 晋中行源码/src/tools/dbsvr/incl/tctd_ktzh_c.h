#ifndef __tctd_ktzh_CH__
#define __tctd_ktzh_CH__
struct tctd_ktzh_c{
	char      rowid[20];
	char      br_no[6];
	char      tctd_br_no[13];
	long      begin_date;
	long      end_date;
	char      tctd_acno[25];
	char      tctd_name[61];
	char      tc_kt_flag[2];
	char      td_kt_flag[2];
	char      cx_kt_flag[2];
	double    td_amt_lmt;
	char      tctd_xyno[61];
	char      tel[5];
	long      trace_no;
	char      filler[61];
};
#endif 
