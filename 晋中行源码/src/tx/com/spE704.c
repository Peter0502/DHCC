/*************************************************
* 文 件 名:  spE704.c
* 功能描述： 未复核交易抹账交易
*
* 作    者:  
* 完成日期： 2004年3月25日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "auth_code_c.h"
#include "tx_log_c.h"

int spE704()
{
	int	ret;
	struct auth_code_c vauth_code;
	struct tx_log_c vtx_log;
	long vau_code;
	char tmpstr[21];

	/* 初始化结构 */
	pub_base_sysinit();

	get_zd_data("0250",tmpstr);		
	vau_code=atol(tmpstr);

	ret=Auth_code_Sel( g_pub_tx.reply, &vauth_code,"auth_no=%d",vau_code);
	if( ret )
	{
		vtcp_log("此授权码不存在: 授权码: [%d]", vau_code);TRACE
		strcpy( g_pub_tx.reply,"Z101" );
		goto ErrExit;
	}

	if (vauth_code.sts[0] == '9')
	{
		sprintf(acErrMsg, "此授权交易已经完成, 不能抹帐!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "Z202");
		goto ErrExit;
	}


	/**
	ret = Tx_log_Sel(g_pub_tx.reply, &vtx_log," trace_no=%d ",
				vauth_code.trace_no );
	if( ret )
	{
		sprintf( acErrMsg,"交易不存在[%d]",vauth_code.trace_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z106" );
		goto ErrExit;
	}
	***/

	ret = sql_execute("delete from auth_code where auth_no=%ld",
			vauth_code.auth_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "删除授权码管理表错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "Z203");
		goto ErrExit;
	}

	set_zd_data("0640",vtx_log.tx_code );

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
