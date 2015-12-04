/*************************************************
* 文 件 名:  sp9984.c
* 功能描述： 由保证金账号回显承兑拆分信息
*
* 作    者:  jane
* 完成日期： 2003年8月25日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_split_po_c.h"

int sp9984()
{
	struct	mdm_ac_rel_c		sMdmAcRel;			/* 介质与账户关系表 */
	struct	mo_bank_split_po_c	sMoBankSplitPo;		/* 贴现登记薄 */
	struct	dd_mst_c			sDdMst;				/* 活期存款主文件 */
	int	ret;

	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );

	get_zd_data( "0330", g_pub_tx.ac_no );	/* 保证金账号 */

	/* 查询帐号是否为本行帐号 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				g_pub_tx.ac_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此客户帐号不存在![%s]", g_pub_tx.ac_no );
   		WRITEMSG
		set_zd_data( "0961", "所输保证金账号有误,请输入承兑协议编号回显相应信息!" );	/* 提示信息 */
TRACE
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* 根据保证金账号ID和帐户序号查询承兑拆分原始票据登记薄 */
	ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, "bail_ac_no='%s'", g_pub_tx.ac_no );
	if( ret == 100 )
	{
		set_zd_data( "0961", "所输保证金账号有误,请输入承兑协议编号回显相应信息!" );	/* 提示信息 */
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_split_po_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* 向前台屏幕赋值 */
	set_zd_data( "0620", sMoBankSplitPo.pact_no );	/* 承兑协议编号 */
	set_zd_data( "0900", "111" );					/* 票据类型 */
	set_zd_data( "0600", sMoBankSplitPo.pnote_no );	/* 票据号 */
	set_zd_double( "1002", sMoBankSplitPo.par_amt );/* 汇票金额 */

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
