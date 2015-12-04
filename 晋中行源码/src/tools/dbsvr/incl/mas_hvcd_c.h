#ifndef __mas_hvcd_CH__
#define __mas_hvcd_CH__
struct mas_hvcd_c{
	char      rowid[20];
	char      trace_no[33];
	char      payficcode[13];
	char      payfictype[2];
	char      payer_name[61];
	char      payer_ac_no[41];
	char      payer_br_no[21];
	char      payer_type[3];
	char      payer_id_type[3];
	char      payer_id_no[21];
	char      payee_name[61];
	char      payee_type[3];
	char      payee_id_type[3];
	char      payee_id_no[21];
	char      cur_no[4];
	char      tx_amt[16];
	char      paydate[9];
	char      use[21];
	char      sttl_type[3];
	char      txcode[11];
	char      extend[21];
};
#endif 
