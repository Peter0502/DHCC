#ifndef __mo_fax_code_CH__
#define __mo_fax_code_CH__
struct mo_fax_code_c{
	char      rowid[20];
	char      bank_no[6];
	char      bank_name[31];
	char      posi_name[11];
	char      phone[13];
	char      tele_code[7];
	char      zip[7];
	char      clr_seq_no[7];
	char      clr_jour_no[7];
	char      clr_lst_occ_date[7];
	char      clr_seq_mode[2];
	char      clr_jour_mode[2];
	char      clr_ctl[4];
};
#endif 
