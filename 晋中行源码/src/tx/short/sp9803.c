/*************************************************
* 文 件 名:  sp9803.c
* 功能描述： 由柜员号回显柜员名称
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
#include "com_tel_c.h"

int sp9803()
{
	struct	com_tel_c	sComTel;		/* 柜员信息表结构 */
	int	ret;

	get_zd_data( "0920", g_pub_tx.tel );		/* 柜员号 */
sprintf(acErrMsg,"tel[%s]",g_pub_tx.tel );
WRITEMSG
	if( strcmp( g_pub_tx.tel, "" ) != 0 )
	{
		ret=Com_tel_Sel( g_pub_tx.reply , &sComTel ,\
			 	"tel='%s'" , g_pub_tx.tel );	
		if( ret==100 )
		{
			sprintf( acErrMsg, "取柜员信息表错!\
				tel=[%s]", g_pub_tx.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply, "S009");
			goto	ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Com_tel_Sel错误!ret=[%d]", ret );
			WRITEMSG
			goto	ErrExit;
		}

		set_zd_data( "0260", sComTel.name );	/* 机构名称 */
		set_zd_data( "0910", sComTel.br_no );	/* 机构号码 20070425 */
		set_zd_data( "0700", sComTel.csts );	/* 柜员状态 20100707 */
	}
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
