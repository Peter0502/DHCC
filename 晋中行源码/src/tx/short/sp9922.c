/*************************************************
* 文 件 名:  sp9922.c
* 功能描述： 由贴现帐号回显贴现信息
*
* 作    者:  jane
* 完成日期： 2003年6月15日
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

int sp9922()
{
	struct	mdm_ac_rel_c	sMdmAcRel;	/* 介质与账户关系表 */
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	struct	dd_mst_c	sDdMst;		/* 活期存款主文件 */
	int	ret;

	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );

	get_zd_data( "0330", g_pub_tx.ac_no );	/* 贴现帐号 */

	pub_base_old_acno( g_pub_tx.ac_no );                 /**对旧帐号的处理**/

	/* 查询客户帐号是否为本行帐号 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				g_pub_tx.ac_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此客户帐号不存在![%s]", g_pub_tx.ac_no );
   		WRITEMSG
		set_zd_data( "0961", "所输贴现账号有误,请输入贴现协议编号回显相应信息!" );	/* 提示信息 */
TRACE
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* 根据贴现帐号ID和帐户序号查询贴现登记薄 */
	ret =Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "ac_id=%ld and ac_seqn=%d", \
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		set_zd_data( "0961", "所输贴现账号有误,请输入贴现协议编号回显相应信息!" );	/* 提示信息 */
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* 向前台屏幕赋值 */
	set_zd_data( "0620", sMoDiscnt.pact_no );	/* 贴现协议编号 */
	set_zd_data( "0900", sMoDiscnt.note_type );	/* 票据类型 */
	set_zd_data( "0600", sMoDiscnt.pnote_no );	/* 票据号 */
	set_zd_double( "1002", sMoDiscnt.par_amt );	/* 汇票金额 */
 
        set_zd_double("0410",sMoDiscnt.par_amt);

	set_zd_data( "0250", sMoDiscnt.pn_name );	/* 转贴现申请人户名 */
	set_zd_data( "0260", sMoDiscnt.pn_name );	/* 贴现申请人户名 */
	set_zd_data( "0270", sMoDiscnt.remitter );	/* 出票人人户名 */
	set_zd_long( "0440", sMoDiscnt.pn_date );	/* 转贴现日期 */
	set_zd_long( "0460", sMoDiscnt.return_date );  /* 回购日期 */
	set_zd_double( "0840", sMoDiscnt.pn_rate );    /* 转贴现率 */
	set_zd_double( "1001", sMoDiscnt.pn_int );      /* 贴现利息 */

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




