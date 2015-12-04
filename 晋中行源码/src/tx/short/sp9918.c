/*************************************************************
* 文 件 名:  sp9918.c
* 功能描述： 大额支付快捷查询
*			 汇票申请退回密押检查
*
* 作    者:  jane
* 完成日期： 2003年6月12日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9918()
{
	char billofxchngseal[11];
	struct mbfebillopnreg_c mbfebillopnreg_v;

	memset(&mbfebillopnreg_v,0x00,sizeof(struct mbfebillopnreg_c));

	get_zd_data("0860",mbfebillopnreg_v.billcode);
	get_zd_data("0880",billofxchngseal);

	g_reply_int=Mbfebillopnreg_Sel(g_pub_tx.reply,&mbfebillopnreg_v,
		"billcode='%s'",mbfebillopnreg_v.billcode);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "查询汇票签发登记簿数据库操作出错![%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if (strcmp(mbfebillopnreg_v.billofxchngseal,billofxchngseal))
	{
		sprintf( acErrMsg, "汇票密押有误，请检查！");
		MBFEWRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
