/*************************************************************
* 文 件 名: spF781.c
* 功能描述：判断是否他行账号业务(长沙城市号为:4302),否则报错
* 作    者: jack
* 完成日期：2003年5月24日
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

spF781()
{
	char cCity[5];
	int ret = 0;

	double tram = 0.00;		/*交易金额*/
	double sxtram = 5.00;	/*手续费金额*/

	struct four_tradelog_c sFour_Tradelog;

	memset(&sFour_Tradelog,0x0,sizeof(sFour_Tradelog));
	memset(cCity,0x0,sizeof(cCity));

	get_zd_data("0360",cCity);
	if( strncmp(cCity, "4302", 4) != 0)	/*-- 目标行为长沙 --*/
	{
		strcpy( g_pub_tx.reply, "F001" );
		sprintf( acErrMsg, "该交易目标行非本行!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	set_zd_data("0310",TESTCARD );	/*内部帐号*/
	set_zd_int("0350",1);				/*活期账号序号*/
	get_zd_double("0400",&tram);


OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"判断目标行交易成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy(g_pub_tx.reply,"F006");
	sprintf(acErrMsg,"判断目标行交易出错[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
