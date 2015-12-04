/*************************************************
* 文 件 名:  sp9775.c
* 功能描述： 由机构号回显机构名称
*            82域的第11位放的是机构级别
* 作    者:  rob 
* 完成日期： 2003年5月21日 
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

int sp9775()
{
	struct	com_branch_c	sComBranch;		/* 公共机构码结构 */
	int	ret;
	char cBrsts[50];
	memset(cBrsts, 0 , sizeof(cBrsts));

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

	set_zd_data( "0830", sComBranch.br_name );	/* 机构名称 */
	memcpy(cBrsts,sComBranch.wrk_sts,sizeof(sComBranch.wrk_sts)-1);
	pub_base_full_space(cBrsts,sizeof(cBrsts)-1);
	cBrsts[10]=sComBranch.br_type[0];
	set_zd_data( "0820", cBrsts);
	vtcp_log("%s,%d brsts===[%s]",__FILE__,__LINE__,cBrsts);

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
