#ifndef __mo_city_CH__
#define __mo_city_CH__
struct mo_city_c{
	char      rowid[20];
	long      tx_date;
	long      tx_no;
	char      tx_br_no[6];
	char      tel[5];
	char      trans_mode[2];
	long      bat_no;
	long      no;
	char      in_out_ind[2];
	char      cd_ind[2];
	char      bill_type[4];
	char      bill_no[17];
	char      payer_ac_no[25];
	char      payer_name[51];
	char      payer_br_no[6];
	char      payer_br_name[41];
	char      payee_ac_no[25];
	char      payee_name[51];
	char      payee_br_no[6];
	char      payee_br_name[41];
	long      sign_date;
	long      matr_date;
	double    bill_amt;
	char      sts[2];
	long      pr_cnt;
	long      tc_date;
	char      brf[51];
	char      remark[51];
};
#endif 
