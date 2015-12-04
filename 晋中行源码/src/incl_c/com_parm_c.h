#ifndef __com_parm_CH__
#define __com_parm_CH__
struct com_parm_c{
	char      rowid[20];
	char      parm_code[6];
	long      parm_seqn;
	char      parm_name[41];
	char      val_type[2];
	char      val[21];
};
#endif 
