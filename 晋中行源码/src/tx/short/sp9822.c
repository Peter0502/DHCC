/*************************************************
* 文 件 名:  sp9822.c
* 功能描述： 由显示账号回显客户姓名 
*			
*
* 作    者:  rob 
* 完成日期： 2003年1月20日 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9822()
{
	int	ret;

	get_zd_data( "0310", g_pub_tx.ac_no ); 			/* 帐号 */
	get_zd_int( "0340",  &g_pub_tx.ac_seqn ); 		/* 账户序号 */

	/*根据显示帐号和账号序号返回账户类型和主文件*/
	ret = pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
								g_pub_tx.ac_id_type);
	if( ret )
	{
		sprintf(acErrMsg,"调用由显示帐号和账号序号返回账户类型和主文件出错!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto	ErrExit;
	}

	switch(g_pub_tx.ac_id_type[0])
	{
		case '1':
				strcpy(g_pub_tx.name,g_dd_mst.name);
				break;
		case '2':
				strcpy(g_pub_tx.name,g_td_mst.name);
				break;
		default	:
				sprintf(acErrMsg,"该类账户不允许冻结!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "O007" );
				goto	ErrExit;
	}

	set_zd_data( "0250", g_pub_tx.name );			/* 账户名称 */

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
