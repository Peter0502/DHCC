#ifndef __rpt_note_ctl_CH__
#define __rpt_note_ctl_CH__
struct rpt_note_ctl_c{
	char      rowid[20];
	long      no;
	char      txname[21];
	char      wherelist[201];
	char      chk_code[101];
	long      txbalmd;
	char      grouplist[81];
	char      tw_acchrt[9];
	char      summary[41];
	char      flag[11];
};
#endif 
