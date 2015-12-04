/*************************************************
* 文 件 名:  sp9985.c
* 功能描述： 由承兑协议编号回显承兑拆分原始票据登记薄信息
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

int sp9985()
{
	struct  mo_bank_split_po_c  sMoBankSplitPo; /* 承兑拆分原始票据登记簿 */
	struct	mdm_ac_rel_c		sMdmAcRel;		/* 介质与账户关系表 */
	int	ret;

	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	
	get_zd_data( "0630", sMoBankSplitPo.pact_no );	/* 质押编号 */

	/* 检查承兑拆分原始票据登记薄中是否存在此承兑协议编号 */
	ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, "pact_no='%s'", sMoBankSplitPo.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "承兑拆分原始票据登记薄中不存在此质押编号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B308" );
		goto ErrExit;
  	}else if( ret )
  	{
		sprintf( acErrMsg, "执行Mo_bank_split_po_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 向前台屏幕赋值 */
	set_zd_double( "1002", sMoBankSplitPo.par_amt );	/* 票面金额 */
	set_zd_data( "0900", "111" );						/* 票据类型 */
	set_zd_data( "0600", sMoBankSplitPo.pnote_no );		/* 票据号 */
	set_zd_data( "0330", sMoBankSplitPo.bail_ac_no );	/* 保证金账号 */

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
