#ifndef __fn_flt_radio_parm_CH__
#define __fn_flt_radio_parm_CH__
struct fn_flt_radio_parm_c{
	char      rowid[20];
	char      prdt_code[21];
	char      ctl_flag[2];
	char      cif_lvl[2];
	double    flt_radio1;
	double    buy_amt;
	double    flt_radio2;
	double    buy_amt1;
	double    flt_radio21;
	double    buy_amt2;
	double    flt_radio22;
	double    buy_amt3;
	double    flt_radio23;
	double    buy_amt4;
	double    flt_radio24;
	double    total_amt;
	double    flt_radio3;
	long      reg_date;
	char      reg_tel[7];
	long      upd_date;
	char      upd_tel[7];
};
#endif 
