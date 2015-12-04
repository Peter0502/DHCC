/*************************************************
* 文 件 名: spF415.c
* 功能描述：异地缴费时的检查(通过同城)
*
* 作    者: mike
* 完成日期: 2004年06月30日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "mo_city_c.h"
#include "mbfeghvrecvlist_c.h"
#include "mo_merge_ac_c.h"

spF415()  
{ 	
	int ret=0;
	char flag[2];

	struct mo_city_c sMo_city_c;
	struct mbfeghvrecvlist_c sMbfeghvrecvlist;
	struct mo_merge_ac_c sMo_merge_ac;

	memset(&sMo_city_c, 0x0, sizeof(struct mo_city_c));
	memset(flag, 0x0, sizeof(flag));
	memset(&sMbfeghvrecvlist, 0x0, sizeof(struct mbfeghvrecvlist_c));
	memset(&sMo_merge_ac, 0x0, sizeof(struct mo_merge_ac_c));

	pub_base_sysinit();

	get_zd_data("0710", flag); /* 操作类型 1 现金 2 转帐 3 同城 4 大额 */

	if (flag[0] == '3') /* 同城 */
	{
		sprintf(acErrMsg, "同城");
		WRITEMSG
		get_zd_data("0300", sMo_city_c.payer_ac_no);
		get_zd_data("0310", sMo_city_c.payee_ac_no);
		get_zd_double("0420", &sMo_city_c.bill_amt);
		sprintf(acErrMsg, "payer [%s],payee [%s], [%lf]", 
			sMo_city_c.payer_ac_no, sMo_city_c.payee_ac_no, 
			sMo_city_c.bill_amt);
		WRITEMSG
		ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city_c, "payer_ac_no='%s' and \
			payee_ac_no='%s' and bill_amt=%lf and in_out_ind='2'", 
			sMo_city_c.payer_ac_no,
			sMo_city_c.payee_ac_no, sMo_city_c.bill_amt);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "查询同城登记簿错误!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg, "没有满足条件的同城业务!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "H028");
			goto ErrExit;
		}

		if (sMo_city_c.sts[0] != 'O' && sMo_city_c.sts[0] != 'P')
		{
			sprintf(acErrMsg, "该笔业务还没有记帐!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "H029");
			goto ErrExit;
		}
	}
	else if (flag[0] == '4') /* 大额  */
	{
		sprintf(acErrMsg, "大额");
		WRITEMSG
		get_zd_data("0300", sMbfeghvrecvlist.payeracc);
		get_zd_data("0310", sMbfeghvrecvlist.recipientacc);
		get_zd_double("0420", &sMbfeghvrecvlist.amount);
		sprintf(acErrMsg, " payer=[%s], payee=[%s], amt=[%lf]",
			sMbfeghvrecvlist.payeracc, sMbfeghvrecvlist.recipientacc,
			sMbfeghvrecvlist.amount);
		WRITEMSG
		ret= Mbfeghvrecvlist_Sel(g_pub_tx.reply, &sMbfeghvrecvlist,
			"payeracc='%s' and recipientacc='%s' and amount=%lf",
			sMbfeghvrecvlist.payeracc, sMbfeghvrecvlist.recipientacc,
			sMbfeghvrecvlist.amount);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "查询大额来账登记簿异常!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			ret = Mo_merge_ac_Sel(g_pub_tx.reply, &sMo_merge_ac, "ac_no='%s'", 
				sMbfeghvrecvlist.recipientacc);
			if ( ret )
			{
				sprintf(acErrMsg, "查询新老账号对照表错误!![%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack(sMo_merge_ac.old_ac_no);
			strcpy(sMbfeghvrecvlist.recipientacc, sMo_merge_ac.old_ac_no);
			ret= Mbfeghvrecvlist_Sel(g_pub_tx.reply, &sMbfeghvrecvlist,
				"payeracc='%s' and recipientacc='%s' and amount=%lf",
				sMbfeghvrecvlist.payeracc, sMbfeghvrecvlist.recipientacc,
				sMbfeghvrecvlist.amount);
			if ( ret == 100)
			{
				sprintf(acErrMsg, "大额来账登记簿中没有该笔业务!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply, "H038");
				goto ErrExit;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "查询大额来账登记簿错误!! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
		}

		if (sMbfeghvrecvlist.procstate[0] != '0' && \
			sMbfeghvrecvlist.procstate[0] != '7')
		{
			sprintf(acErrMsg, "该笔来账还没有处理!! [%s]",
				sMbfeghvrecvlist.procstate);
			WRITEMSG
			strcpy(g_pub_tx.reply, "H039");
			goto ErrExit;
		}
	}
	else 
	{
		sprintf(acErrMsg, "操作类型错误!! [%s]", flag);
		WRITEMSG
		strcpy(g_pub_tx.reply, "H037");
		goto ErrExit;
	}

	strcpy(g_pub_tx.brf, "财政非税代缴");
	if (pub_ins_trace_log()) 
	{ 
		sprintf(acErrMsg,"登记交易流水错误!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"异地缴费检查处理成功OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"异地缴费检查处理失败!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
