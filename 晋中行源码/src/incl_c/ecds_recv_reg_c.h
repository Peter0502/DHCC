#ifndef __ecds_recv_reg_CH__
#define __ecds_recv_reg_CH__
struct ecds_recv_reg_c{
	char      rowid[20];
	long      ecds_tx_date;
	char      ecds_trace_no[29];
	long      trace_no;
	long      tx_date;
	char      drftcls[2];
	char      drfttyp[5];
	char      drftnb[31];
	long      issedt;
	long      duedt;
	char      tag_name[31];
	char      sts[2];
	char      check_flag[2];
	char      bf_flag[2];
};
#endif 
