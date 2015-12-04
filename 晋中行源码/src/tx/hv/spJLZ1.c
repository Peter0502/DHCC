/*************************************************
* 文 件 名:	spJLZ1
* 功能描述：大额业务来帐挂账会滚及设置挂账科目
* 作    者: LiuHuafeng
* 完成日期: 2006-8-30 18:24
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

spJLZ1()
{
	char cAc_no[22];
	char cCheckStat[2];
	int iRet=0;
	char cKinbr[BRNO_LEN+1];
	memset(cKinbr, 0 , sizeof(cKinbr));
	memset(cAc_no , 0 , sizeof(cAc_no));
	memset(cCheckStat, 0 , sizeof(cCheckStat));
	get_zd_data("0980",cCheckStat);
	if(memcmp(g_pub_tx.reply,"0000",4))
	{
		/* 表明上一个函数记帐失败,所以要会滚数据 */
		sql_rollback();
	}
	else
	{
		/* 检查函数检查出来的挂装状态 */
		sql_rollback();
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
