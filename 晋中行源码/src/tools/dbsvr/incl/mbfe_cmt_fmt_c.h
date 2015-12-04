#ifndef __mbfe_cmt_fmt_CH__
#define __mbfe_cmt_fmt_CH__
struct mbfe_cmt_fmt_c{
	char      rowid[20];
	char      cmtno[4];
	char      cmt_name[21];
	char      head_flag[3];
	char      oper_type[2];
	char      send_period[6];
	char      regtable[21];
	char      od_tx_code[5];
	char      rd_tx_code[5];
};
#endif 
