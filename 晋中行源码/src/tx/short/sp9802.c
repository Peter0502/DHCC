/*************************************************
* 文 件 名:  sp9802.c
* 功能描述： 由机构号回显机构名称
*
* 作    者:  jane
* 完成日期： 2003年1月12日 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

int sp9802()
{
	struct	com_branch_c	sComBranch;		/* 公共机构码结构 */
	int	ret;

	get_zd_data( "0910", g_pub_tx.tx_br_no );	/* 机构号 */

	ret=Com_branch_Sel( g_pub_tx.reply , &sComBranch ,\
			 "br_no='%s'" , g_pub_tx.tx_br_no );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "取公共机构码表错!\
			br_no=[%s]", g_pub_tx.tx_br_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S019" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Com_branch_Sel错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	set_zd_data( "0250", sComBranch.br_name );	/* 机构名称 */
	set_zd_data( "0680", sComBranch.wrk_sts );	/* 营业状态 */

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
