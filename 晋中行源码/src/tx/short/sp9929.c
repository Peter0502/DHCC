/*************************************************************
* 文 件 名:  sp9929.c
* 功能描述： 大额支付快捷查询
*			 汇票签发汇票号码检查
*
* 作    者:  jane
* 完成日期： 2003年6月20日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"

int sp9929()
{
	char billcode[9];

	pub_base_sysinit();

	get_zd_data( "0860", billcode );

	/*检查该机构该柜员是否有该种类型凭证的这一张*/
	g_reply_int = pub_com_ChkNtExt( billcode, billcode, "110", g_pub_tx.tx_br_no, g_pub_tx.tel );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用函数检查汇票号码错误!");
		WRITEMSG
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
