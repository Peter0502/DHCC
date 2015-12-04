/***********************************************************
* 文 件 名: spF707.c
* 功能描述：柜员异地交易最后一笔流水查询
* 作    者: jack
* 完成日期：2003年2月22日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "four_tradelog_c.h"

spF707()
{
	int ret = 0;
	char cTel[5];

	struct four_tradelog_c sFour_Tradelog;
	
	memset(&sFour_Tradelog,0x0,sizeof(sFour_Tradelog));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	memset(cTel, 0x0, sizeof(cTel));
	get_zd_data("0360", cTel);
	get_zd_data("0920",sFour_Tradelog.tx_tel);
	sprintf(acErrMsg,"------------> 柜员号为[%s]", sFour_Tradelog.tx_tel );
	WRITEMSG
	sprintf(acErrMsg,"------------> 柜员号为[%s]", cTel );
	WRITEMSG
    	

	ret = sql_count("four_tradelog","tx_date = %ld and tx_tel = '%s'", \
		g_pub_tx.tx_date, sFour_Tradelog.tx_tel);
	if (ret == 0)
	{
		sprintf(acErrMsg,"无要找的交易流水![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	if (ret >0)
	{
		ret = 0;
		ret = sql_max_long( "four_tradelog","trace_no",&sFour_Tradelog.trace_no, \
			"tx_date = %ld and tx_tel = '%s'",g_pub_tx.tx_date, sFour_Tradelog.tx_tel);
		if (ret != 0)
		{
			sprintf(acErrMsg,"取本行柜员异地交易最大流水号出错![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"F002");
			goto ErrExit;
		}
	}

	ret = Four_tradelog_Sel( g_pub_tx.reply, &sFour_Tradelog, "tx_date = %ld and trace_no = %ld", \
		g_pub_tx.tx_date, sFour_Tradelog.trace_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询要找的流水记录失败![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("0610",sFour_Tradelog.tx_code);
	set_zd_long("0530",sFour_Tradelog.tx_date);
	set_zd_long("0540",sFour_Tradelog.tx_time);
	set_zd_data("0300",sFour_Tradelog.ac_no);
	set_zd_double("0400",sFour_Tradelog.tx_amt);
	set_zd_double("0850",sFour_Tradelog.sx_rate);
	set_zd_long("0550",sFour_Tradelog.trace_no);
	set_zd_data("0210",sFour_Tradelog.cur_no);
	if (strcmp(sFour_Tradelog.tx_stat,"0") == 0)
	{
		set_zd_data("0650","正常");
	}
	else
	{
		set_zd_data("0650","撤销");
	}


OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询流水处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询流水处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
