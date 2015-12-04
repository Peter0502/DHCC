#ifndef __mo_loss_CH__
#define __mo_loss_CH__
struct mo_loss_c{
	char      rowid[20];
	char      ac_no[20];
	char      id_type[2];
	char      id_no[21];
	char      name[61];
	char      loss_ind[2];
	char      prop_book_loss[2];
	char      unloss_ind[2];
	char      opn_br_no[6];
	long      loss_date;
	char      loss_br_no[6];
	char      loss_tel[5];
	char      loss_chk[5];
	char      loss_auth[5];
	long      unloss_date;
	char      unloss_tel[5];
	char      unloss_chk[5];
	char      unloss_auth[5];
	char      new_ac_no[20];
	long      wrk_date;
	long      trace_no;
};
#endif 
