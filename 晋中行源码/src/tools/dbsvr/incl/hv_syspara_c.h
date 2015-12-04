#ifndef __hv_syspara_CH__
#define __hv_syspara_CH__
struct hv_syspara_c{
	char      rowid[20];
	char      sdate[9];
	char      updtype[2];
	double    startamt;
	char      timedc[9];
	char      tdcname[21];
	char      updtdc[19];
	char      updmethod[2];
	char      datatype[2];
};
#endif 
