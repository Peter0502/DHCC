#ifndef __czcust_CH__
#define __czcust_CH__
struct czcust_c{
	char      rowid[20];
	char      opnbr[8];
	double    ac_id;
	char      ac_no[20];
	double    cif_no;
	char      name[81];
	char      pact_no[21];
	double    avbal;
	char      opday[9];
	char      endday[9];
	char      id_no[21];
	char      crd_no[21];
	char      tablename[21];
	char      flag[2];
	char      id_type[2];
	long      ncif_no;
};
#endif 
