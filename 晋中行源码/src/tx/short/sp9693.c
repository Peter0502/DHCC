/*************************************************
* 文 件 名:  sp9693.c
* 功能描述： 由贴现协议编号回显贴现登记薄信息
*
* 作    者:  rob 
* 完成日期： 2003年9月27日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9693()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* 介质与账户关系表 */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	
	get_zd_data( "0900", sMoDiscnt.note_type );	/* 承兑汇票种类 */
	get_zd_data( "0600", sMoDiscnt.pnote_no );	/* 承兑汇票号码 */

	/* 检查贴现登记薄中是否存在此贴现协议编号 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "note_type='%s' and pnote_no='%s' and pn_type='4' ", sMoDiscnt.note_type,sMoDiscnt.pnote_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此票据信息!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
   	}
	else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	/* 向前台屏幕赋值 */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* 异地同城标志 */
	set_zd_data( "0250", sMoDiscnt.tpn_name );	/* 贴现申请人户名 */
	set_zd_double( "0840", sMoDiscnt.tpn_rate );	/* 转贴现率 */
	set_zd_double( "1001", sMoDiscnt.tpn_int );	/* 贴现利息 */

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
