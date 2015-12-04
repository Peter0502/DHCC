/*************************************************
* 文 件 名:  spB344.c
* 功能描述： 完成承兑拆分原始票据票款收回功能；
*
* 作    者:  jane
* 完成日期： 2003年8月25日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_split_po_c.h"
#include "com_parm_c.h"

int spB344()
{
	struct  mo_bank_split_po_c  sMoBankSplitPo; /* 承兑拆分原始票据登记簿 */
	char	cPayAcType[2];					/* 还款帐户类型 */
	int		ret;

	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( cPayAcType, 0x00, sizeof( cPayAcType ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0630", sMoBankSplitPo.pact_no );	/* 质押编号 */
vtcp_log("质押编号:[%s]",sMoBankSplitPo.pact_no);
	get_zd_data( "0670", cPayAcType );				/* 还款帐户类型 */

	/* 根据质押编号查找承兑拆分原始票据登记簿 */
	ret = Mo_bank_split_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankSplitPo.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_split_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_bank_split_po_Fet_Upd( &sMoBankSplitPo, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "承兑拆分原始票据登记薄中不存在此质押编号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B308" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_bank_split_po_Fet_Upd错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	/* 检查状态是否为录入 */
	if( sMoBankSplitPo.sts[0] == '2' )
	{
		sprintf( acErrMsg, "此张承兑汇票已做票款收回!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B304" );
		goto ErrExit;
	}

	/* 修改承兑拆分原始票据登记薄 */
	strcpy( sMoBankSplitPo.pay_type, cPayAcType );	/* 还款帐户类型 */
	strcpy( sMoBankSplitPo.sts, "2" );				/* 2 ：票款收回 */

	ret = Mo_bank_split_po_Upd_Upd( sMoBankSplitPo, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改承兑拆分原始票据登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_bank_split_po_Clo_Upd();

vtcp_log( "janejanejane   [0450]=[%ld]", sMoBankSplitPo.matr_date );
	set_zd_long("0450",sMoBankSplitPo.matr_date);		/* 票据到期日期，为授权所加 */

	/* 记流水日志 */
	/**--- xxx --- @20030818@ ---- 将协议编号付给帐号撤销时用 ---**/
	strcpy(g_pub_tx.ac_no,sMoBankSplitPo.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.brf,"承兑拆分原始票据票款收回");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	
	if( cPayAcType[0] == '1' )		/* 本地帐户 */
	{
		sprintf(acErrMsg,"本地帐户");
		WRITEMSG
		goto	OkExit1;
	}else if( cPayAcType[0] == '2' )	/* 异地帐户 */
	{		
		sprintf(acErrMsg,"异地帐户");
		WRITEMSG
		goto	OkExit2;
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
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
