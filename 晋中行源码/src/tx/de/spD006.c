/***************************************************************
* 文 件 名: spD006.c
* 功能描述:	判断转账方账号类型
* 作    者: jack
* 完成日期：2004年6月1日
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
#include "prdt_ac_id_c.h"

spD006()
{
	int ret;
	struct prdt_ac_id_c g_prdt_ac_id;

	get_zd_data("1011",g_pub_tx.ac_no);
	get_zd_data("1171",g_pub_tx.ct_ind);

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", \
						  g_pub_tx.ac_no);
	if( ret )	
	{
		sprintf(acErrMsg,"介质账号对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W012");
		goto ErrExit;
	}

	/*
	if( g_mdm_ac_rel.ac_seqn==9999 )
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!");
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==1 )
		{ ; }
	}
	*/

	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id, "ac_id=%ld", \
						  g_mdm_ac_rel.ac_id);
	if( ret )	
	{
		sprintf(acErrMsg,"产品账户对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W012");
		goto ErrExit;
	}

	if( g_prdt_ac_id.prdt_no[0]=='2' )
	{
		sprintf(acErrMsg,"不可以从定期户进行转帐!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O180");
		goto ErrExit;
	}

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"判断账户类型失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"判断账户类型成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
