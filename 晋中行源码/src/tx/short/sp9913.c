/*************************************************
* 文 件 名:  sp9913.c
* 功能描述： 由承兑汇票号码回显承兑汇票信息
*
* 作    者:  jane
* 完成日期： 2003年5月30日
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
#include "mo_bank_acc_po_c.h"
#include "com_parm_c.h"

int sp9913()
{
	struct	com_parm_c		wd_com_parm;		/* 承兑汇票登记簿 */
	struct	mo_bank_po_c		sMoBankPo;		/* 承兑汇票登记簿 */
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* 银行承兑汇票基本信息登记薄 */
	char	cPoNo[17];							/* 承兑汇票号码屏幕取值 */
	char	cPoNo1[9];							/* 承兑汇票号码有效位 */
	char	cPoNoHead[9];						/* 承兑汇票号码冠字头 */
	char	cPoNo2[17];							/* 承兑汇票号码冠字头+有效位 */
	int		ret=0;
	int     parm_ind = 0;
	char	flag[2];							/* 承兑汇票号码冠字头+有效位 */

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );

	pub_base_sysinit();

	get_zd_data( "0580", cPoNo );		/* 承兑汇票号码 */
	vtcp_log("[%s][%d]58输入的承兑汇票号码是[%s]\n",__FILE__,__LINE__,cPoNo);
	get_zd_data( "0720", flag );		/* 承兑汇票号码 */

	/* 检查所输承兑汇票号码是否带冠字头 */
	if(flag[0]!='1')
	ret = pub_com_ChkNoteHead1( "111", cPoNo );
	if( ret )
	{
		sprintf( acErrMsg, "检查承兑汇票号码出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]582输入的承兑汇票号码是[%s]\n",__FILE__,__LINE__,cPoNo);

	/* 取承兑汇票冠字头 */
	strncpy( cPoNoHead, cPoNo, 8 );
	cPoNoHead[8]='\0';
	pub_base_strpack( cPoNoHead );

	/* 取承兑汇票有效位 */
	strncpy( cPoNo1, cPoNo+8, 8 );
	cPoNo1[8] = '\0';
sprintf( acErrMsg, "xxxx    cPoNo1[%s]", cPoNo1 );
WRITEMSG

	/* 承兑汇票号码冠字头+有效位 */
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo1 );
sprintf( acErrMsg, "xxxx    cPoNo2[%s]", cPoNo2 );
WRITEMSG
	pub_base_strpack_all( cPoNo2 );

	/* 根据承兑汇票号码查找银行承兑汇票登记薄 */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo2 );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此承兑汇票!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B272" );
		goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_bank_po_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoBankPo.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此承兑协议编号!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B187" );
		goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 向前台屏幕赋值 */	
	set_zd_data( "0260", sMoBankAccPo.payer_name );		/* 付款人全称 */
	set_zd_data( "0380", sMoBankAccPo.payer_ac_no );	/* 付款人帐号 */
	set_zd_data( "0300", sMoBankAccPo.payer_br_no );	/* 付款人开户行行号 */
	set_zd_data( "0250", sMoBankAccPo.payer_br_name );	/* 付款人开户行行名 */
	set_zd_data( "0270", sMoBankAccPo.payee_name );		/* 收款人全称 */
	set_zd_data( "0830", sMoBankAccPo.payee_ac_no );	/* 收款人账号 */
	set_zd_data( "0810", sMoBankAccPo.payee_br_name );	/* 收款人开户行行名 */
	set_zd_double( "1001", sMoBankAccPo.sign_amt );		/* 签发票面金额 */
	set_zd_long( "0440", sMoBankAccPo.matr_date );		/* 到期日期 */
	set_zd_double( "1003", sMoBankAccPo.bail_amt );		/* 保证金金额 */
	set_zd_double( "1002", sMoBankAccPo.tot_mort_amt );	/* 质押总金额 */
	set_zd_data( "0320", sMoBankAccPo.bail_ac_no );		/* 保证金帐号 */
	set_zd_data( "0670", sMoBankAccPo.acc_type);		/* 汇票类型 */
	set_zd_double( "0400", sMoBankAccPo.bail_amt_ratio );	/* 保证金比例 */
	set_zd_long( "0450", sMoBankPo.receive_date );		/* 托收日期 */

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
