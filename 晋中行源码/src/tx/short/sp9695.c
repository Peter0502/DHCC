/*************************************************
* 文 件 名:  sp9695.c
* 功能描述： 保证金利率维护快捷查询
*
* 作    者:   
* 完成日期： 2003年11月02日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bail_rate_c.h"

int sp9695()
{
	struct	mo_bail_rate_c	sMoBailRate;		/* 保证金利率登记薄 */
	char    czm[2];
	int	ret;

	/* 数据初始化 */
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
    pub_base_sysinit();
	get_zd_data( "0680", czm );							/* 操作码 */
	get_zd_data( "0670", sMoBailRate.bail_type );		/* 保证金种类 */
	get_zd_data( "0330", sMoBailRate.pact_no );			/* 协议编号 */

	if( czm[0]=='0' )		/*** 增加 ***/
	{
		strcpy(g_pub_tx.reply,"0000");
		return 0;
	}

	if( sMoBailRate.bail_type[0]=='1' )				/** 承兑保证金 **/
	{
		ret=Mo_bail_rate_Sel(g_pub_tx.reply,&sMoBailRate,"pact_no='%s' and bail_type[1]='1' and sts='1'",sMoBailRate.pact_no);
	}
	else if( sMoBailRate.bail_type[0]=='2' )		/** 承兑保证金 **/
	{
		ret=Mo_bail_rate_Sel(g_pub_tx.reply,&sMoBailRate,"pact_no='%s' and bail_type[1]='2' and sts='1'",sMoBailRate.pact_no);
	}
	else
	{
   		sprintf( acErrMsg, "保证金类型错误![%d][%s][%s]",ret,sMoBailRate.pact_no,sMoBailRate.bail_type );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if( ret )
   	{
   		sprintf( acErrMsg, "登记保证金利率登记薄错误![%d][%s][%s]",ret,sMoBailRate.pact_no,sMoBailRate.bail_type );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	set_zd_data( "0320", sMoBailRate.bail_ac_no );		/* 保证金帐号 */
	set_zd_data( "0310", sMoBailRate.ac_no );			/* 收息帐号 */
	set_zd_double( "0390",sMoBailRate.amt );			/* 保证金金额 */
	set_zd_long( "0440", sMoBailRate.beg_date );		/* 起息日期 */
	set_zd_long( "0450", sMoBailRate.end_date );		/* 到期日期 */
	set_zd_double( "0840",sMoBailRate.ic_rate );		/* 利率 */

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
