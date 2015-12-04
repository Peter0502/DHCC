/*************************************************
* 文 件 名:  sp9974.c
* 功能描述： 保兑仓保证金账号快查
*
* 作    者:  jane
* 完成日期： 2003年8月8日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"

int sp9974()
{
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* 银行承兑汇票基本信息登记薄 */
	struct	mo_bank_po_c		sMoBankPo;	/* 承兑汇票登记簿 */
	double	dPoAmt;					/* 已签发票据总金额 */
	char	cPoType[2];				/* 承兑类型 */
	char	cBailAcNo[25];				/* 保证金帐号 */
	double	dBailAmtRatio;				/* 保证金比例 */
	int	ret;

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cPoType, 0x00, sizeof( cPoType ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );

	get_zd_data( "0330", sMoBankAccPo.pact_no );	/* 承兑协议编号 */
	get_zd_data( "0320", cBailAcNo );		/* 保证金帐号 */
	get_zd_data( "0670", cPoType );			/* 承兑类型 */
	get_zd_double( "0400", &dBailAmtRatio );/* 保证金比例 */
sprintf( acErrMsg, "sMoBankAccPo.pact_no[%s]cBailAcNo[%s]cPoType[%s]",sMoBankAccPo.pact_no,cBailAcNo,cPoType );
WRITEMSG
	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoBankAccPo.pact_no );
	if( ret == 100 )
	{
		ret = pub_base_CompDblVal( dBailAmtRatio, 0.00 );
		if( ret == 1 )
		{
			sprintf( acErrMsg, "如果选择保证金承兑，则必须先做缴存保证金交易!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B264" );
			goto ErrExit;
		}
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	pub_base_strpack( sMoBankAccPo.bail_ac_no );
sprintf( acErrMsg, "sts[%s]", sMoBankAccPo.acc_po_sts );
WRITEMSG
	if( sMoBankAccPo.acc_po_sts[0] != 'A' && sMoBankAccPo.acc_po_sts[0] != 'B' )
	{
		sprintf( acErrMsg, "此承兑协议编号已存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B191" );
		goto ErrExit;
	}

	/* 检查承兑类型为保兑仓，则需要检查保证金账号及金额 */
sprintf( acErrMsg, "sMoBankAccPo.pact_no[%s]cBailAcNo[%s]bail_ac_no[%s]",sMoBankAccPo.pact_no,cBailAcNo,sMoBankAccPo.bail_ac_no );
WRITEMSG	
	if( strcmp( sMoBankAccPo.bail_ac_no, cBailAcNo ) != 0 )
	{
		sprintf( acErrMsg, "保证金账号必须与缴存保证金时录入一致!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B258" );
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
