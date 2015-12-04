#ifndef __trace_parm_CH__
#define __trace_parm_CH__
struct trace_parm_c{
	char      rowid[20];
	char      tx_code[5];
	long      seq_no;
	char      field[21];
	char      field_type[11];
	char      field_name[51];
	char      list[51];
};
#endif 
