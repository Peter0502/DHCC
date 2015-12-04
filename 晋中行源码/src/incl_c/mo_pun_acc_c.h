#ifndef __mo_pun_acc_CH__
#define __mo_pun_acc_CH__
struct mo_pun_acc_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      sts;
	long      beg_date;
	long      end_date;
	char      beg_tel[5];
	char      beg_br_no[6];
	char      end_tel[5];
	char      end_br_no[6];
	char      brf[51];
};
#endif 
