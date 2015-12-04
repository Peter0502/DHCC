/***************************************************************
* 文 件 名: spD269.c
* 功能描述: 清算核对
*
* 作    者: jack
* 完成日期: 2004年6月20日
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


spD269()
{
	double dr_amt=0.00, cr_amt=0.00, amt1=0.00, amt2=0.00, tc_amt=0.00;
	char err_msg[51];
	int ret=0;

	struct com_parm_c sCom_parm;

	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	memset(err_msg, 0x0, sizeof(err_msg));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_count("mo_city", "sts='2'");
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计手工打印票据错误!!");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		sprintf(acErrMsg, "还有手工打印票据没有核销!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D221");
		goto ErrExit;
	}

	ret = sql_count("trace_log", "tx_date=%ld and tx_br_no='%s' and \
		tx_code='4269'", g_pub_tx.tx_date, g_pub_tx.tx_br_no);
	if (ret < 0) 
	{ 
		sprintf(acErrMsg, "统计流水表异常 [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	}
	else if (ret == 0)
	{
		ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
			parm_seqn=1");
		if (ret != 0)
		{
			sprintf(acErrMsg, "查询COM_PARM异常!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else 
	{
		ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
			parm_seqn=2");
		if (ret != 0)
		{
			sprintf(acErrMsg, "查询COM_PARM异常!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}

	pub_base_strpack(sCom_parm.val);
	tc_amt = atof(sCom_parm.val);
	sprintf(acErrMsg, "昨日408贷方余额为:[%s],[%lf]", sCom_parm.val, tc_amt);
	WRITEMSG

	/* 昨日同城未提出录入金额 */
	/* 统计未核销的提出借方金额 */
	ret = sql_sum_double("mo_city", "bill_amt", &dr_amt, "in_out_ind='1' and \
		cd_ind = '1' and sts in ('1', '2', '6')");
	if (ret != 0)
	{
		sprintf(acErrMsg, " 统计未提出借方票据金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	/* 统计未核销的提出贷方金额 */
	ret = sql_sum_double("mo_city", "bill_amt", &cr_amt, "in_out_ind='1' and \
		cd_ind = '2' and sts in ('1', '2', '6') ");
	if (ret != 0)
	{
		sprintf(acErrMsg, " 统计未提出贷方票据金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	/* 未提出票据金额 */
	amt1 = cr_amt - dr_amt;

	sprintf(acErrMsg, "未提出票据金额为 : [%lf]", amt1);
	WRITEMSG

	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code = 'TCKM' and \
			parm_seqn = 2");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(sCom_parm.val);

	/* 统计408外科目的借方金额 */
	ret = sql_sum_double("dc_log", "amt", &dr_amt, "acc_hrt = '%s' and \
			dc_ind = '1' and sts = '0'", sCom_parm.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计408外科目的借方金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	sprintf(acErrMsg, "408外部科目的借方金额为: [%lf]", dr_amt);
	WRITEMSG

	/* 统计408外科目的贷方金额 */
	ret = sql_sum_double("dc_log", "amt", &cr_amt, "acc_hrt = '%s' and \
			dc_ind = '2' and sts = '0'", sCom_parm.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计408外科目的贷方金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	sprintf(acErrMsg, "统计408外科目的贷方金额为: [%lf]", cr_amt);
	WRITEMSG

	amt2 = cr_amt + tc_amt - dr_amt;
	sprintf(acErrMsg, "统计408外部清算科目的余额为: [%lf]", amt2);
	WRITEMSG

	if (pub_base_CompDblVal(amt1, amt2) != 0)
	{
		sprintf(acErrMsg, "同城登记簿中未核销金额与408外部清算科目金额不符");
		WRITEMSG
		sprintf(acErrMsg, "同城登记簿中的金额为:[%lf],408外部清算科目:[%lf]",
			amt1, amt2);
		WRITEMSG
		sprintf(err_msg, "核对不平:贷:[%.2lf], 借:[%.2lf]", 
			cr_amt+tc_amt-amt1, dr_amt);
		set_zd_data("0130", err_msg);
		strcpy(g_pub_tx.reply, "HDBP");
		goto ErrExit;
	}

	ret = Com_parm_Dec_Upd(g_pub_tx.reply, "parm_code='GZJE' and parm_seqn=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询COM_PARM异常!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	ret = Com_parm_Fet_Upd(&sCom_parm, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "FETCH ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(sCom_parm.val, "%lf", amt1);
	sprintf(acErrMsg, "本日未清算金额:[%s], [%lf]", sCom_parm.val, amt1);
	ret = Com_parm_Upd_Upd(sCom_parm, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "UPDATE ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	Com_parm_Clo_Upd();

	/*** 登记交易流水 ***/ 
	strcpy(g_pub_tx.brf, "清算核对"); 
	if( pub_ins_trace_log() ) 
	{ 
		sprintf(acErrMsg,"登记交易流水错误!"); 
		WRITEMSG 
		goto ErrExit; 
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"清算核对成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"清算核对失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

