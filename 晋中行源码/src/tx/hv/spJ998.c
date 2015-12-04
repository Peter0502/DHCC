/*************************************************
* 文 件 名:  spJ998.c
* 功能描述： 大额来账 挂帐检查 子交易
*			       
*			 
* 作    者:  ChenMing
* 完成日期： 2006年8月30日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

int spJ998()
{

	char cStat[2];
	memset(cStat,0,sizeof(cStat));
	pub_base_sysinit();
	vtcp_log("spJ998() 大额来账 挂帐检查 子交易 started! ");  
	if (pub_hv_recv_check_main())
	{
		vtcp_log("%s,%d pub_hv_recv_check_main 返回挂账处理情况",__FILE__,__LINE__);
		goto ErrExit; /* 调用 大额来帐挂帐检查 主函数 */
	}
	get_zd_data("0980",cStat);
	vtcp_log("[%s][%d]  cStat=====[%s]",__FILE__,__LINE__,cStat);

	/* strcpy( g_pub_tx.reply,"0000" ); */


OkExit:
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

