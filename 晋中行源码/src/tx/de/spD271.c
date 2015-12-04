/***************************************************************
* 文 件 名: spD271.c
* 功能描述: 查询书录入
*
* 作    者: jack
* 完成日期: 2004年4月14日
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
#include "mo_city_query_c.h"

spD271()
{
    int ret;

    struct mo_city_query_c sMo_city_query;

    memset(&sMo_city_query, 0x0, sizeof(struct mo_city_query_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0910",sMo_city_query.rec_br_no);

	ret = sql_count("com_branch", "br_no='%s' and wrk_sts!= '*'", 
		sMo_city_query.rec_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "查询公共机构码表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 0)
	{
		sprintf(acErrMsg, "没有该接受机构!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D193");
		goto ErrExit;
	}

	ret = sql_count("com_tx_br_rel", "br_no='%s' and up_br_no='%s' and \
		rel_type=3", g_pub_tx.tx_br_no, sMo_city_query.rec_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "查询交易机构关系表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 0)
	{
		ret = sql_count("com_tx_br_rel", "br_no='%s' and up_br_no='%s' and \
			rel_type=3", sMo_city_query.rec_br_no, g_pub_tx.tx_br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "查询交易机构用关系表错误!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 0)
		{
			sprintf(acErrMsg, "交易机构和接受机构不能相互查询!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D192");
			goto ErrExit;
		}
	}

    /* 登记查询查复登记簿 */
    sMo_city_query.tx_date = g_pub_tx.tx_date;
    sMo_city_query.tx_no = g_pub_tx.trace_no;
    strcpy(sMo_city_query.tx_br_no, g_pub_tx.tx_br_no);
    strcpy(sMo_city_query.tel, g_pub_tx.tel);

	get_zd_data("0670", sMo_city_query.type);
	get_zd_long("0440", &sMo_city_query.note_date);
	get_zd_data("0680", sMo_city_query.note_type);
	get_zd_double("0390", &sMo_city_query.tx_amt);
	get_zd_data("0300", sMo_city_query.payer_ac_no);
	get_zd_data("0250", sMo_city_query.payer_name);
	get_zd_data("0270", sMo_city_query.payer_br_name);
	get_zd_data("0310", sMo_city_query.payee_ac_no);
	get_zd_data("0260", sMo_city_query.payee_name);
	get_zd_data("0830", sMo_city_query.payee_br_name);
	get_zd_data("0760", sMo_city_query.content);
	strcpy(sMo_city_query.sts, "0");

    ret = Mo_city_query_Ins(sMo_city_query, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记查询查复登记簿错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }


	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "查询书录入");
	strcpy(g_pub_tx.no_show, "1");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询查复台帐管理处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询查复台帐管理处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

