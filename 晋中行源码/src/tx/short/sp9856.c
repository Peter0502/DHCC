/*************************************************
* 文 件 名:  sp9856.c
* 功能描述： 由协议编号回显再贷款登记薄信息
*
* 作    者:  jane
* 完成日期： 2003年2月21日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_again_ln_c.h"

int sp9856()
{
	struct	mo_again_ln_c	sMoAgainLn;	/* 再贷款登记薄 */
	int	ret;

	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	
	get_zd_data( "0580", sMoAgainLn.pact_no );	/* 协议编号 */

	/* 检查再贷款登记薄中是否存在此协议编号 */
	ret = Mo_again_ln_Sel( g_pub_tx.reply, &sMoAgainLn, "pact_no='%s'", sMoAgainLn.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "再贷款登记薄中不存在此协议编号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B107" );
                goto ErrExit;
  }else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_again_ln_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 向前台屏幕赋值 */
	set_zd_double( "1001", sMoAgainLn.amt );		/* 贷款金额 */
	set_zd_long( "0450", sMoAgainLn.reg_date );   /* 登记日期 */
	set_zd_long( "0440", sMoAgainLn.to_date );		/* 到期日期 */
	set_zd_double( "0840", sMoAgainLn.rate );		/* 利率 */
	vtcp_log("wjwjw[%lf]",sMoAgainLn.rate);
	set_zd_data( "0670", sMoAgainLn.again_ln_type );/* 再贷款类型 */

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




