/*************************************************
* 文 件 名:  spJ314.c
* 功能描述： 退回应答来报子交易
* 作    者:  jane
* 完成日期： 2004年6月25日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/

#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

int spJ314()
{
	g_reply_int=sql_execute("update mbfereturnapp set procstate='00' where rtnappno='%s' and consigndate='%s' and odficode='%s'",
				mbfetran.origrtnappno,mbfetran.origrtnappdate,
				mbfetran.rdficode);
	if ( g_reply_int )
	{
		sprintf(acErrMsg,"修改原退回申请处理状态出错[%d]",g_reply_int);
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
