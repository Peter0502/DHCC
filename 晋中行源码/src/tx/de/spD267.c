/***************************************************************
* 文 件 名: spD267.c
* 功能描述: 支行对中心清算
*
* 作    者: jack
* 完成日期: 2004年4月04日
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
#include "com_tx_br_rel_c.h"

spD267()
{
    int ret;
    char flag[2];
	double tc_d_amt=0.00, tc_c_amt=0.00, ye_d_amt=0.00, ye_c_amt=0.00;
	double tc_d_amt_z=0.00, tc_c_amt_z=0.00;
	char err_msg[61];
    struct com_parm_c com_parm_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(err_msg, 0x0, sizeof(err_msg));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0670",flag);

    /* 检查是否是支行对中心 */
    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=4 and \
          br_no='%s'", g_pub_tx.tx_br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询交易机构关系表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"该机构不能执行此交易!! ",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D156");
        goto ErrExit;
    }

	/* 408外科目 */
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	pub_base_strpack(com_parm_c.val); /* 408外 */

	ret = sql_count("mo_city", "tx_date=%ld and \
		sts = '2' and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s')",
		g_pub_tx.tx_date, g_pub_tx.tx_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计同城登记簿异常!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		sprintf(acErrMsg, "本中心还有手工打印票据没有核销[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D221");
		goto ErrExit;
	}

	/* 统计分中心没有提出金额 */
	tc_d_amt = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &tc_d_amt, "tx_date=%ld and \
		sts in ('1', '6') and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s') and cd_ind='1'", 
		g_pub_tx.tx_date, g_pub_tx.tx_br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计同城登记簿中的提出金额异常!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** 本日未提出贷方金额为: [%lf]", tc_d_amt);
	WRITEMSG

	tc_c_amt = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &tc_c_amt, "tx_date=%ld and \
		sts in ('1', '6') and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s') and cd_ind='2'", 
		g_pub_tx.tx_date, g_pub_tx.tx_br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计同城登记簿中的提出金额异常!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** 本日未提出借方金额为: [%lf]", tc_c_amt);
	WRITEMSG

	tc_c_amt_z = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &tc_c_amt_z, "tx_date!=%ld and \
		sts in ('3', '7') and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s') and cd_ind='2' \
		and tc_date=%ld", 
		g_pub_tx.tx_date, g_pub_tx.tx_br_no, g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计同城登记簿中的提出金额异常!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** 本日提出他日票据借方金额为: [%lf]", 
		tc_c_amt_z);
	WRITEMSG

	tc_d_amt_z = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &tc_d_amt_z, "tx_date!=%ld and \
		sts in ('3', '7') and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s') and cd_ind='1' \
		and tc_date=%ld", 
		g_pub_tx.tx_date, g_pub_tx.tx_br_no, g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计同城登记簿中的提出金额异常!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** 本日提出他日票据借方金额为: [%lf]", 
		tc_d_amt_z);
	WRITEMSG

	/* 统计分中心提入金额 */

	/* 统计分中心的408外余额 */
	/* 借408外 */
	ye_d_amt = 0.00;
	ret = sql_sum_double("dc_log", "amt", &ye_d_amt, "tx_date=%ld and \
		tx_tx_br_no in (select br_no from com_tx_br_rel where rel_type=3 and \
		up_br_no='%s') and sts='0' and \
		dc_ind='1' and acc_hrt='%s'", g_pub_tx.tx_date, g_pub_tx.tx_br_no,
		com_parm_c.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计408外借方金额错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** 408外借方金额为 [%lf]", ye_d_amt);
	WRITEMSG

	ye_c_amt = 0.00;
	ret = sql_sum_double("dc_log", "amt", &ye_c_amt, "tx_date=%ld and \
		tx_tx_br_no in (select br_no from com_tx_br_rel where rel_type=3 and \
		up_br_no='%s') and sts='0' and \
		dc_ind='2' and acc_hrt='%s'", g_pub_tx.tx_date, g_pub_tx.tx_br_no,
		com_parm_c.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计408外贷方余额错误! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** 408外贷方余额: [%lf]", ye_c_amt);
	WRITEMSG

	sprintf(acErrMsg, "CGH****** 408外余额为: [%lf]", 
		ye_c_amt - ye_d_amt);
	WRITEMSG

	if (flag[0] == '1')
	{
		g_pub_tx.tx_amt1 = ye_c_amt - ye_d_amt - g_pub_tx.tx_amt1;
	}
	else 
	{
		g_pub_tx.tx_amt1 = ye_c_amt - ye_d_amt + g_pub_tx.tx_amt1;
	}

	sprintf(acErrMsg, "CGH****** 408外余额为: [%lf]", g_pub_tx.tx_amt1);
	WRITEMSG
	g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 + tc_c_amt_z - tc_d_amt_z;

	if (pub_base_CompDblVal(g_pub_tx.tx_amt1, tc_c_amt - tc_d_amt) != 0)
	{
		/*
		sprintf(acErrMsg, "清算科目余额不正确!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D223");
		*/
		sprintf(acErrMsg, "同城登记簿中的金额为:[%lf],408外部清算科目:[%lf]", 
			g_pub_tx.tx_amt1, tc_c_amt - tc_d_amt); 
		WRITEMSG 

		if (pub_base_CompDblVal(g_pub_tx.tx_amt1, tc_c_amt - tc_d_amt) < 0)
		{
			sprintf(err_msg, "核对不平:40801贷方少记金额:[%.2lf]", 
				tc_c_amt-tc_d_amt-g_pub_tx.tx_amt1); 
		}
		else
		{
			sprintf(err_msg, "核对不平:40801借方少记金额:[%.2lf]", 
				g_pub_tx.tx_amt1 - tc_c_amt + tc_d_amt); 
		}
		set_zd_data("0130", err_msg); 
		strcpy(g_pub_tx.reply, "HDBP"); 
		goto ErrExit;
	}

	/****** 
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (flag[0] == '2')
    {
        * 提出 借408外 *
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","支行对中心清算记帐");
    }
    else
    {
        * 提入 贷408外 *
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        set_zd_data("1215","2");
        set_zd_data("121A","支行对中心清算记帐");
    }

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    if (flag[0] == '2')
    {
        * 提出 贷内部清算科目 *
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        set_zd_data("1215","2");
        set_zd_data("121A","支行对中心清算记帐");
    }
    else
    {
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","支行对中心清算记帐");
    }
	**/
    
	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "支行对中心清算");
    get_zd_double("0390",&g_pub_tx.tx_amt1);
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"支行对中心清算处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"支行对中心清算处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

