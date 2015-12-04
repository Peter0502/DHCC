/***************************************************************
* 文 件 名: spD308.c
* 功能描述: 判断是否是财政帐号
*
* 作    者: mike
* 完成日期: 2004年1月16日
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
#include "mo_city_c.h"
#include "com_parm_c.h"

spD308()
{
	int ret = 0;

	struct com_parm_c sCom_parm;

	pub_base_sysinit();

	get_zd_data("1011", g_pub_tx.ac_no);
	sprintf(acErrMsg, "---------帐号:[%s]", g_pub_tx.ac_no);
	WRITEMSG
    ret=Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='CZZH' and \
        val='%s'", g_pub_tx.ac_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	else if ( ret == 0 )
	{
		sprintf(acErrMsg, "该帐号不能做存款交易!![%s]", g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D261");
		/* set_zd_data("0130", "该帐号不能做此交易,请[6400]中的交易"); */
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"该帐号不是财政帐号[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"该帐号是财政帐号[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

