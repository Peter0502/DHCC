#ifndef __mas_hvpt_CH__
#define __mas_hvpt_CH__
struct mas_hvpt_c{
	char      rowid[20];
	char      trace_no[33];
	char      payficcode[13];
	char      payfictype[2];
	char      payer_name[61];
	char      payer_ac_no[41];
	char      payer_br_no[21];
	char      origrdficode[13];
	char      origrdfictype[2];
	char      payee_name[61];
	char      payee_ac_no[41];
	char      payee_br_no[21];
	char      cur_no[4];
	char      tx_amt[16];
	char      paydate[9];
	char      recvdate[9];
	char      use[21];
	char      sttl_type[3];
	char      notecode[33];
	char      txcode[11];
	char      extend[21];
};
#endif 
