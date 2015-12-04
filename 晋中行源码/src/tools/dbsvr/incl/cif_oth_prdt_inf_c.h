#ifndef __cif_oth_prdt_inf_CH__
#define __cif_oth_prdt_inf_CH__
struct cif_oth_prdt_inf_c{
	char      rowid[20];
	long      cif_no;
	long      inf_no;
	char      prdt_code[3];
	char      unit_type[3];
	char      unit_no[7];
	char      ac_no[25];
	char      pro_cur_no[3];
	double    balance;
	double    mon_pay;
};
#endif 
