#ifndef __note_parm_CH__
#define __note_parm_CH__
struct note_parm_c{
	char      rowid[20];
	char      note_type[4];
	char      acc_hrt[6];
	char      name[21];
	char      sell_ind[2];
	char      draw_pwd_ind[2];
	char      dc_ind[2];
	char      ct_ind[2];
	char      ctl_no_ind[2];
	char      order_ind[2];
	char      cur_no[3];
	double    unit_amt;
	char      dc_unit_mode[2];
	char      use_ind[3];
	char      cif_type[2];
	long      unit_cnt;
	long      ocp_cnt;
	long      buy_unit;
	long      buy_cnt;
	long      unit_lmt;
	long      cnt_lnt;
	long      head_len;
};
#endif 
