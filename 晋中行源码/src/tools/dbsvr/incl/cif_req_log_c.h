#ifndef __cif_req_log_CH__
#define __cif_req_log_CH__
struct cif_req_log_c{
	char      rowid[20];
	char      advice_type[2];
	long      cif_no;
	long      seq_no;
	char      req_clerk_no[6];
	char      request[41];
	long      date;
	char      clerk_no[6];
};
#endif 
