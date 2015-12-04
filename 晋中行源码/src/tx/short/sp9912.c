/*************************************************
* 文 件 名:  sp9912.c
* 功能描述： 由承兑协议编号回显承兑汇票信息
*
* 作    者:  jane
* 完成日期： 2003年5月27日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"

int sp9912()
{
	struct	mo_bank_po_c		sMoBankPo;	/* 承兑汇票登记簿 */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );

	get_zd_data( "0330", sMoBankPo.pact_no );	/* 承兑协议编号 */
	get_zd_long( "0500", &sMoBankPo.pact_no_num );	/* 原编号 */

	/* 根据承兑协议编号查找银行承兑汇票登记薄 */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "pact_no='%s'\
				and pact_no_num=%d", sMoBankPo.pact_no, sMoBankPo.pact_no_num );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此承兑协议编号!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B187" );
		goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_bank_po_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 向前台屏幕赋值 */
	set_zd_long( "0450", sMoBankPo.sign_date );		/* 签发日期 */

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
