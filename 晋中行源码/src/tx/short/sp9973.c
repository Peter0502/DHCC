/*************************************************
* 文 件 名:  sp9973.c
* 功能描述： 大额支付快捷查询
*			 来帐转发根据行号查询机构号再查询机构名称
*
* 作    者:  jane
* 完成日期： 2003年8月08日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_branch_c.h"

int sp9973()
{
	struct mbfebrbkno_c mbfebrbkno_v;
	struct com_branch_c com_branch_v;

	memset( &mbfebrbkno_v, 0x00, sizeof( struct mbfebrbkno_c ) );
	memset( &com_branch_v, 0x00, sizeof( struct com_branch_c ) );

	get_zd_data( "0300", mbfebrbkno_v.bkno );	/*转发至行号*/

	pub_base_sysinit();

	if (get_bkno()) goto ErrExit;

	if( !strcmp( mbfebrbkno_v.bkno, mbfetran.cnapsodfi ) )
	{
		sprintf( acErrMsg, "不可转发至本行!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int = Mbfebrbkno_Sel( g_pub_tx.reply, &mbfebrbkno_v, "bkno='%s'", mbfebrbkno_v.bkno );
	if ( g_reply_int==100 )
	{
		sprintf( acErrMsg, "非本行参与者行号!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "查询机构行行号对照表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &com_branch_v, "br_no='%s'", mbfebrbkno_v.brno );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询公共机构表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0250", com_branch_v.br_name );

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
