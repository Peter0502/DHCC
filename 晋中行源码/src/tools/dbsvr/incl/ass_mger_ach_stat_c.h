#ifndef __ass_mger_ach_stat_CH__
#define __ass_mger_ach_stat_CH__
struct ass_mger_ach_stat_c{
	char      rowid[20];
	long      date;
	char      br_no[6];
	char      mang_no[5];
	double    save_amt;
	double    ln_amt;
	double    ln_prvd_amt;
	double    ln_norm_amt;
	double    ln_over_amt;
	double    ln_stgn_amt;
	double    ln_die_amt;
	double    intst_pay_intst;
	double    intst_lo_intst;
};
#endif 
