/*************************************************
* 文 件 名:  sp9858.c
* 功能描述： 由贴现协议编号回显贴现帐户户名和贴现账户余额
*
* 作    者:  jane
* 完成日期： 2003年2月24日
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

int sp9858()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	struct	ln_mst_c	sLnMst;		/* 贷款主文件 */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sLnMst, 0x00, sizeof( struct ln_mst_c ) );
	
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* 贴现协议编号 */

	/* 检查贴现登记薄中是否已经存在此贴现协议编号 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此贴现协议编号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B113" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 根据贷款账号id和账号序号检查贷款主文件 */
	ret = Ln_mst_Sel( g_pub_tx.reply, &sLnMst, "ac_id=%ld and ac_seqn=%d", sMoDiscnt.ac_id,\
			 sMoDiscnt.ac_seqn );
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
sprintf( acErrMsg, "name=[%s],bal=[%f]", sLnMst.name, sLnMst.bal);
                WRITEMSG	
	/* 向前台屏幕赋值 */
	set_zd_data( "0260", sLnMst.name );             /* 贴现帐户户名 */
    	set_zd_double( "1004", sLnMst.bal );		/* 贴现账户余额 */
    	set_zd_double( "1001", sMoDiscnt.par_amt );	/* 汇票金额 */

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




