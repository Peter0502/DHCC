/***********************************************************
* 文 件 名: spF702.c
* 功能描述：判断要被冲正的记录是否存在
* 作    者: jack
* 完成日期：2003年2月21日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "four_tradelog_c.h"

spF702()
{
	int ret = 0;
	int trace_no = 0;
	char cTrace_no[9];
	int iTrace_no = 0;
	char cCity[5];
	
	struct four_tradelog_c sFour_Tradelog;

	double tram = 0.00;		/*交易金额*/

	g_reply_int=0;

	memset(cCity,0x0,sizeof(cCity));
	memset(&sFour_Tradelog,0x0,sizeof(sFour_Tradelog));
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0360",cCity);
	get_zd_double("0400",&tram);
	
	memset( cTrace_no, 0x0, sizeof(cTrace_no) );
	get_zd_data( "0610", cTrace_no );
	iTrace_no = atol( cTrace_no );

	sprintf( acErrMsg,"---->要被冲正的后台流水号为[%ld]\n", iTrace_no );
	WRITEMSG

	ret = Four_tradelog_Sel(g_pub_tx.reply, &sFour_Tradelog, \
		"tx_date = %ld and city_no = '%s' and trace_no = %ld \
		and tx_amt = %f and tx_stat = '0'", \
		g_pub_tx.tx_date, cCity, iTrace_no, tram);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询被撤销交易记录失败![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0370", sFour_Tradelog.ac_no );

	ret = Four_tradelog_Dec_Upd( g_pub_tx.reply , "tx_date = %ld \
		and city_no = '%s' and trace_no = %ld and tx_amt = %f \
		and tx_stat = '0'",g_pub_tx .tx_date,cCity, \
		sFour_Tradelog.trace_no, tram);
	if (ret != 0)
	{
		sprintf(acErrMsg,"修改被撤销交易记录失败111![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	ret = Four_tradelog_Fet_Upd( &sFour_Tradelog , &g_pub_tx.reply );
	if (ret != 0)
	{
		sprintf(acErrMsg,"修改被撤销交易记录失败222![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	
	strcpy(sFour_Tradelog.tx_stat,"1");

	ret = Four_tradelog_Upd_Upd( sFour_Tradelog , &g_pub_tx.reply  );
	if (ret != 0)
	{
		sprintf(acErrMsg,"修改被撤销交易记录失败333![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	ret = Four_tradelog_Clo_Upd( );

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"异地撤销记录处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	Four_tradelog_Clo_Upd( );
	sprintf(acErrMsg,"异地撤销记录处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
