/***************************************************************
* 文 件 名: gD281.c
* 功能描述: 40710转入到114帐户0828
*
* 作    者: rob
* 完成日期: 2003年6月24日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

char  def_br_no[6];
char  def_tel[5];

gD281()
{
    int ret=0;
	double c_bal=0.00, d_bal=0.00;
	char dc_ind;
	struct com_parm_c sCom_parm;

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	memset(def_br_no, 0x0, sizeof(def_br_no));
	memset(def_tel, 0x0, sizeof(def_tel));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));

	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");
	strcpy(g_pub_tx.tx_br_no, def_br_no);
	strcpy(g_pub_tx.tel, def_tel);

	sprintf(acErrMsg, "-------->交易日期为:[%ld]", g_pub_tx.tx_date);
	WRITEMSG

	/* 汇总借方发生额 */
	ret = sql_sum_double("dc_log_rz", "amt", &c_bal, "acc_hrt='407A0' and \
		tx_date=%ld and dc_ind='1' and sts='0'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总40710的借方金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_sum_double("dc_log_rz", "amt", &d_bal, "acc_hrt='407A0' and \
		tx_date=%ld and dc_ind='2' and sts='0'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总40710的贷方金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	sprintf(acErrMsg, "40710的借方金额为:[%lf], 40710的贷方金额为:[%lf]",
		c_bal, d_bal);
	WRITEMSG

	strcpy(g_pub_tx.cur_no, "01");
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.brf, "通兑清算0828补");
	c_bal = 300.00; /* 补20030828 */
	d_bal = 0.00;
	if (pub_base_CompDblVal(c_bal, d_bal) == 1)
	{
		/* 借方多 */
		strcpy(g_pub_tx.add_ind, "1"); /* 40710减少 */
		g_pub_tx.tx_amt1 = c_bal - d_bal;
		dc_ind = '1';
	}
	else if (pub_base_CompDblVal(c_bal, d_bal) == -1)
	{
		/* 贷方多 */
		strcpy(g_pub_tx.add_ind, "0"); /* 借40710增加 */
		g_pub_tx.tx_amt1 = d_bal - c_bal;
		dc_ind = '0';
	}
	else
	{
		sprintf(acErrMsg, "本日40710的总发生额为零!!");
		WRITEMSG
		goto OkExit;
	}

	strcpy(g_pub_tx.ac_no, "40710"); 
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	/* 记40710会计帐 */
	if (dc_ind == '0')
	{
		/* 内部借40710 A016*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
	}
	else if (dc_ind == '1')
	{
		/* 内部贷 A017 40710*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "借贷标志错误!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}
	if (ret != 0)
	{
		sprintf(acErrMsg, "记40710会计帐错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg, "*******114 ************");
	WRITEMSG

	/* 记114账号 */
	memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c)); 
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}
	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");
	strcpy(g_pub_tx.tx_br_no, def_br_no);
	strcpy(g_pub_tx.tel, def_tel);
	strcpy(g_pub_tx.cur_no, "01"); 
	strcpy(g_pub_tx.ct_ind, "2"); 
	strcpy(g_pub_tx.brf, "通兑清算0828补");
	if (dc_ind == '0') /* 借 */
	{
		strcpy(g_pub_tx.add_ind, "1");
		g_pub_tx.tx_amt1 = d_bal - c_bal;
	}
	else /* 贷 */
	{
		strcpy(g_pub_tx.add_ind, "0");
		g_pub_tx.tx_amt1 = c_bal - d_bal;
	}
	sprintf(acErrMsg, "------------>[%lf]", g_pub_tx.tx_amt1);
	WRITEMSG

	ret= Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='YDTD' and \
		parm_seqn=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(sCom_parm.val);
	strcpy(g_pub_tx.ac_no, sCom_parm.val);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	sprintf(acErrMsg, "--------->[%s]", g_pub_tx.add_ind);
	WRITEMSG
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	if (dc_ind == '1') /* 40710贷 */
	{
		/* 内部借114 A016*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
	}
	else if (dc_ind == '0')
	{
		/* 内部贷114 A017*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "借贷标志错误!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}
	if (ret != 0)
	{
		sprintf(acErrMsg, "记114账号会计帐错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"40710转入114账号处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"40710转入114账号处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
