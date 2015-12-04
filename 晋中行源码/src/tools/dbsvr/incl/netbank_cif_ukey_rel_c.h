#ifndef __netbank_cif_ukey_rel_CH__
#define __netbank_cif_ukey_rel_CH__
struct netbank_cif_ukey_rel_c{
	char      rowid[20];
	long      cif_no;
	char      ukey[17];
	char      sts[2];
	long      sign_date;
	long      cancel_date;
};
#endif 
