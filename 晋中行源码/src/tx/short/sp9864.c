/*************************************************
* 文 件 名:  sp9864.c
* 功能描述： 由贴现协议编号回显贴现还款登记薄信息
*
* 作    者:  jane
* 完成日期： 2003年3月9日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_pay_discnt_c.h"

int sp9864()
{
	struct	mo_pay_discnt_c	sMoPayDiscnt;	/* 贴现还款登记薄 */
	struct	ln_mst_c	sLnMst;		/* 贷款主文件 */
	int	ret;

	memset( &sMoPayDiscnt, 0x00, sizeof( struct mo_pay_discnt_c ) );
	memset( &sLnMst, 0x00, sizeof( struct ln_mst_c ) );
	
	get_zd_data( "0620", sMoPayDiscnt.pact_no );/* 贴现协议编号 */

	/* 检查贴现登记薄中是否存在此贴现协议编号 */
	ret = Mo_pay_discnt_Sel( g_pub_tx.reply, &sMoPayDiscnt, "pact_no='%s'", sMoPayDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现还款登记薄中不存在此贴现协议编号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B113" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_pay_discnt_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 根据贷款账号id和账号序号检查贷款主文件 */
	ret = Ln_mst_Sel( g_pub_tx.reply, &sLnMst, "ac_id=%ld and ac_seqn=%d", sMoPayDiscnt.ac_id,\
			 sMoPayDiscnt.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贷款主文件中不存在此账号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "W018" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Ln_mst_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 向前台屏幕赋值 */
	set_zd_data( "0260", sLnMst.name );             /* 贴现帐户户名 */
    	set_zd_double( "1004", sLnMst.bal );		/* 贴现账户余额 */
    	set_zd_double( "1003", sMoPayDiscnt.pay_amt );	/* 还款金额 */
	set_zd_data( "0700", sMoPayDiscnt.ct_ind );	/* 现转标志 */
	if( sMoPayDiscnt.ct_ind[0] == '2' )		/* 转帐时 */
	{
		set_zd_data( "0300", sMoPayDiscnt.turn_ac_no );	/* 还款账号 */
		set_zd_data( "0250", sMoPayDiscnt.turn_name );	/* 户名 */
		goto OkExit;
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




