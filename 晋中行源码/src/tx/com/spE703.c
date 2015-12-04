/*************************************************
* 文 件 名:  spE703.c
* 功能描述： 授权易
*
* 作    者:  andy
* 完成日期： 2004年3月9日
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

int spE703()
{
	int	ret;
	struct auth_code_c vauth_code;
	struct tx_log_c vtx_log;
	long vau_code;
	char tmpstr[21];

	/* 初始化结构 */
	pub_base_sysinit();

	get_zd_data("0330",tmpstr);		
	vau_code=atol(tmpstr);

	sprintf(acErrMsg,"code[%d]",vau_code);

	ret=Auth_code_Sel( g_pub_tx.reply,&vauth_code,"auth_no=%d",vau_code);
	if( ret )
	{
		vtcp_log("此授权码不存在");TRACE
		strcpy( g_pub_tx.reply,"Z101" );
		goto ErrExit;
	}
	if( vauth_code.sts[0]=='4' || vauth_code.sts[0]=='3' )
	{
		vtcp_log("此授权码已经授权");TRACE
		strcpy( g_pub_tx.reply,"Z10J" );
		goto ErrExit;
	}
	if( vauth_code.sts[0]=='9' )
	{
		vtcp_log("此授权码已经完成");TRACE
		strcpy( g_pub_tx.reply,"Z104" );
		goto ErrExit;
	}
vtcp_log("tel[%s] code[%s]",vauth_code.tel,vauth_code.tx_code);
	set_zd_data("0920",vauth_code.tel);
	set_zd_data("0640",vauth_code.tx_code);
OkExit:
    strcpy( g_pub_tx.reply, "Z100" );
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
