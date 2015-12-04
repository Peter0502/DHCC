#ifndef __com_item_CH__
#define __com_item_CH__
struct com_item_c{
	char      rowid[20];
	char      acc_no[8];
	char      acc_hrt[6];
	char      app_cod[6];
	char      up_acc_hrt[6];
	char      acc_name[31];
	char      sub_acc_yn[2];
	char      acc_lvl[2];
	char      dc_ind[2];
	char      roll_ind[2];
	char      acc_knd[2];
	char      fore_ind[2];
	char      equa_ind[2];
	char      amt_dc_ind[2];
	char      od_ind[2];
	char      in_ind[2];
	char      scope[2];
};
#endif 
