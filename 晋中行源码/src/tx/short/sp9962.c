/*************************************************
* 文 件 名:  sp9962.c
* 功能描述： 输入柜员号，回显柜员名称----柜员交接用
*
* 作    者:  rob
* 完成日期： 2003年07月15日
*
* 修改记录： 
*  日   期: 20070429
* 修 改 人: wanglei
* 修改内容: 当交接状态为收入时，校验付出方密码
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int sp9962()
{
	int ret=0;
	char tel[5];
	char cTxtype[2]; /****交接类型 1-付出 2-收入 3-收回****/
	char cPasswd[7]; /****柜员密码****/

	struct com_tel_c g_com_tel;

	memset( &g_com_tel, 0x00, sizeof( struct com_tel_c ) );
	memset( cTxtype, 0x00, sizeof(cTxtype) );
	memset( cPasswd, 0x00, sizeof(cPasswd) );

	get_zd_data( "0920", tel );					/* 柜员号 */
	get_zd_data( "0700", cTxtype );					/* 交接类型 */

	ret = Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",tel);
	if( ret )
	{
		sprintf(acErrMsg,"不存在该柜员!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if(cTxtype[0] == '2')/****收入时，检查付出柜员的密码****/
	{
		get_zd_data( "0790", cPasswd );		/* 柜员密码 */
		pub_base_EnDes(g_pub_tx.tx_date,"",cPasswd);
		if(memcmp(g_com_tel.pwd,cPasswd,6) != 0)
		{
			sprintf(acErrMsg,"对方柜员密码错误!!tel=[%s]",tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O123");
			goto ErrExit;
		}
	}/****根据晋中要求，付出时，对方柜员必须是签退状态  add by wanglei 20070501****/
	else if(cTxtype[0] == '1' && g_com_tel.csts[0] != '2')
	{
		sprintf(acErrMsg,"对方柜员不是签退状态!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P089");
		goto ErrExit;
	}

	set_zd_data( "0250", g_com_tel.name );		/* 柜员名称 */

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
