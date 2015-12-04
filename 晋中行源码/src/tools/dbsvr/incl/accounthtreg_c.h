#ifndef __accounthtreg_CH__
#define __accounthtreg_CH__
struct accounthtreg_c{
	char      rowid[20];
	char      br_no[6];
	long      trace_no;
	long      seqno;
	char      change_no[5];
	char      br_ame[61];
	char      note_type[4];
	char      note_no[17];
	char      pay_ac_no[33];
	char      pay_name[61];
	double    amt;
	char      cash_ac_no[33];
	char      cash_name[61];
	char      cash_opn_br_no[13];
	char      cash_chg_no[5];
	char      cash_opn_name[61];
	char      put_flag[2];
	long      tx_date;
	long      tx_time;
	char      stat[2];
	char      tel[5];
	char      chk[5];
	char      pwd[7];
};
#endif 
