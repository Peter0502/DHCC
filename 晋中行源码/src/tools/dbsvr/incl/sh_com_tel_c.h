#ifndef __sh_com_tel_CH__
#define __sh_com_tel_CH__
struct sh_com_tel_c{
	char      rowid[20];
	char      tel[5];
	char      br_no[6];
	char      name[9];
	char      pwd[7];
	char      lvl[2];
	char      csts[2];
	long      pwd_date;
	char      beg_menu[5];
	char      chk_ind[2];
	char      tx_type[11];
};
#endif 
