#ifndef __new_jz_prdt_CH__
#define __new_jz_prdt_CH__
struct new_jz_prdt_c{
	char      rowid[20];
	char      pact_no[21];
	char      ac_no[20];
	char      ac_name[61];
	char      old_prdt_no[4];
	char      old_prdt_name[61];
	char      new_prdt_no[4];
	char      new_prdt_name[61];
	char      id_no[21];
	double    bj_amt;
	double    in_lo_intst;
	double    out_lo_intst;
	char      sts[2];
	long      ac_id;
	long      tx_date;
	long      tx_time;
	long      trace_no;
	char      tel[5];
	char      tx_br_no[6];
	double    amt1;
	double    amt2;
	char      code1[61];
	char      code2[61];
};
#endif 
