#ifndef __march_info_CH__
#define __march_info_CH__
struct march_info_c{
	char      rowid[20];
	char      mrchno[16];
	char      mrchnm[41];
	char      br_no[6];
	char      mrchtype[3];
	char      mrchstat[2];
	char      mrchacno[25];
	char      mrchacno_ls[25];
	char      mrchaddr[41];
	char      mrchtel[17];
	char      mrchman[11];
	double    chrg;
	long      reg_date;
	long      beg_date;
	long      end_date;
};
#endif 
