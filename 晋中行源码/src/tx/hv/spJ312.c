/*************************************************
* 文 件 名:	spJ312.c
* 功能描述：撤销应答来报子交易
* 作    者: jane
* 完成日期: 2004年6月25日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/

#define	EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

spJ312()
{
	if (!strcmp(mbfetran.cancelresult,"0"))	 /*返回应答为撤销*/
	{
		g_reply_int=sql_execute("update mbfeghvsendlist set procstate='30' where txssno='%s' and consigndate='%s'",
					mbfetran.origtxssno,mbfetran.consigndate);
		if (g_reply_int)
		{
			sprintf(acErrMsg,"修改原大额往账处理状态出错 [%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
	}

	g_reply_int=sql_execute("update mbfeoprtcancel set cancelresult='%s',resultstrinfo='%s',procstate='00' where cancelno='%s' and consigndate='%s'",
				mbfetran.cancelresult,mbfetran.strinfo,
				mbfetran.origrtnappno,mbfetran.consigndate);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"修改业务撤销登记簿出错 [%d]",g_reply_int);
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
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
