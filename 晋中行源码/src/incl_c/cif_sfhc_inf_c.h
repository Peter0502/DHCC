#ifndef __cif_sfhc_inf_CH__
#define __cif_sfhc_inf_CH__
struct cif_sfhc_inf_c{
	char      rowid[20];
	long      cif_no;
	char      name[21];
	long      g_bir;
	long      n_bir;
	char      sex[2];
	char      marriage[2];
	long      mar_date;
	char      edu[2];
	char      religion[2];
	char      nationality[4];
	char      country[4];
	char      folk[3];
	char      language[2];
	char      clerk_no[6];
	char      health[2];
	char      technical_post[3];
	char      headship[3];
	char      birth_adr[41];
	double    mon_income;
	double    h_mon_income;
	long      home_cnt;
};
#endif 
