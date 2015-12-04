#ifndef __mas_hvcs_CH__
#define __mas_hvcs_CH__
struct mas_hvcs_c{
	char      rowid[20];
	char      trace_no[33];
	char      payer_name[61];
	char      payer_id_type[3];
	char      payer_id_no[21];
	char      origrdficode[13];
	char      origrdfictype[2];
	char      payee_name[61];
	char      payee_ac_no[41];
	char      ayee_type[3];
	char      payee_br_no[21];
	char      payee_id_type[3];
	char      payee_id_no[21];
	char      cur_no[4];
	char      tx_amt[16];
	char      recvdate[9];
	char      cash_source[21];
	char      sttl_type[3];
	char      txcode[11];
	char      extend[21];
};
#endif 
