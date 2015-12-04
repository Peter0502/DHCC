#ifndef __lv_pkghtr_CH__
#define __lv_pkghtr_CH__
struct lv_pkghtr_c{
	char      rowid[20];
	char      pkgno[4];
	long      jz_date;
	long      in_date;
	long      wt_date;
	long      pack_date;
	char      orderno[9];
	long      tx_time;
	char      txnum[6];
	char      or_br_no[13];
	char      pay_qs_no[13];
	char      sendco[5];
	char      ac_br_no[13];
	char      cash_qs_no[13];
	char      receco[5];
	double    tx_amt;
	char      pay_opn_br_no[13];
	char      pay_ac_no[33];
	char      pay_name[61];
	char      pay_addr[61];
	char      cash_opn_br_no[13];
	char      cash_ac_no[33];
	char      cash_name[61];
	char      cash_addr[61];
	char      lv_yw_ind[13];
	char      lv_brf[61];
	long      resp_date;
	char      rcpstat[3];
	char      resp1[9];
	char      resp2[9];
	char      note_type[3];
	char      note_no[13];
	char      addid[9];
	char      br_no[8];
	char      input_tel[7];
	char      check_tel[7];
	char      auth_tel[7];
	char      send_tel[7];
	char      resp_tel[7];
	char      deal_tel[7];
	char      input_trace_no[7];
	char      check_trace_no[7];
	char      send_trace_no[7];
	char      resp_trace_no[7];
	char      deal_trace_no[7];
	char      lv_sts[2];
	char      beg_sts[2];
	char      operlevel[2];
	char      tx_chrg_ind[2];
	double    chrg_amt;
	char      checkflag[2];
	char      brprtflag[65];
	char      qsprtflag[65];
	char      packid[9];
	char      retcode[3];
	char      o_txnum[6];
	char      ywdtlid[9];
	char      protno[61];
	char      ac_type[2];
	char      refno[21];
	char      o_or_br_no[13];
	long      o_wt_date;
	long      o_pack_date;
	char      o_orderno[13];
	char      o_packid[9];
	char      o_ac_br_no[13];
	double    df_chrg_amt;
	char      lv_o_sts[2];
	char      lw_ind[2];
	char      cur_no[4];
	char      filler[61];
};
#endif 
