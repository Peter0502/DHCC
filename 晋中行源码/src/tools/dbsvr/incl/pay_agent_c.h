#ifndef __pay_agent_CH__
#define __pay_agent_CH__
struct pay_agent_c{
	char      rowid[20];
	char      unit_no[5];
	char      item_no[21];
	char      name[51];
	char      id_type[2];
	char      id_no[21];
	char      ac_no[20];
	long      trace_no;
	long      end_date;
	long      tx_date;
	double    tram;
	char      remark[41];
	char      ckmk_flag[2];
	char      sata[2];
	char      result[2];
	char      result_name[41];
	char      cnt[3];
};
#endif 
