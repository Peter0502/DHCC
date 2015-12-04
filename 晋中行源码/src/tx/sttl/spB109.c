/*************************************************
* 文 件 名:  spB109.c
* 功能描述： 登记兑付行发出的查询请求内容，以及查询状况。
*
* 作    者:  jane
* 完成日期： 2003年1月19日
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
#include "mo_bank_po_c.h"
#include "mo_po_qry_c.h"

int spB109()
{
	struct	mo_bank_po_c	sMoBankPo;		/* 银行汇票登记簿 */
	struct	mo_po_qry_c	sMoPoQry;		/* 查询查复登记薄 */
	
	char	cPoNo[17];				/* 汇票号码 */
	char	cBrf[201];				/* 内容 */
	char	cOperCode[6];				/* 业务代码 */
	int	ret;
	
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoPoQry, 0x00, sizeof( struct mo_po_qry_c ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cBrf, 0x00, sizeof( cBrf ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0590", cPoNo );
	get_zd_data( "0810", cBrf );
	strcpy( cOperCode, "10001" );
	
	/* 根据汇票号码查找汇票登记薄 */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo );
	if( ret == 100 )
    	{
        	sprintf(acErrMsg,"查找汇票登记薄错!,无此汇票信息!po_no=[%s]",cPoNo);
        	WRITEMSG
       	 	strcpy( g_pub_tx.reply, "B012" );
        	goto ErrExit;
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Mo_bank_po_Sel错误!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
    	}
	
	/* 检查交易机构是否是汇票的签发行 */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查机构权限错!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 为查询查复登记簿结构赋值 */
	strcpy( sMoPoQry.qry_typ, "0" );			/* 查询查复类型为0查询 */
	sMoPoQry.tx_date = g_pub_tx.tx_date;			/* 交易日期 */
	strcpy( sMoPoQry.brf, cBrf );				/* 内容 */
	/*strcpy( sMoPoQry.sts, "" );				 状态 */
	strcpy( sMoPoQry.tel, g_pub_tx.tel );			/* 柜员 */
	strcpy( sMoPoQry.br_no, g_pub_tx.tx_br_no );		/* 机构 */
	
	/* 登记查询查复登记簿 */
	ret = pub_sttl_RegMoPoQry( sMoPoQry );
	if ( ret )
    	{
        	sprintf( acErrMsg, "执行pub_sttl_RegMoPoQry错误!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
    	}
	
	/* 记流水日志 */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
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
