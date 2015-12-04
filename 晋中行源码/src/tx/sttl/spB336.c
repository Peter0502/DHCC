/*************************************************
* 文 件 名:  spB336.c
* 功能描述： 银行收到委托收款后进行托收登记。
*
* 作    者:  jane
* 完成日期： 2003年5月30日
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
#include "mo_bank_acc_po_c.h"

int spB336()
{
	struct	mo_bank_po_c		sMoBankPo;		/* 承兑汇票登记簿 */
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* 承兑汇票基本信息登记簿 */
	char	cPoNo1[17];							/* 承兑汇票号码屏幕取值 */
	char	cPoNo[9];							/* 承兑汇票号码有效位 */
	char	cPoNoHead[9];						/* 承兑汇票号码冠字头 */
	char	cPoNo2[17];							/* 承兑汇票号码冠字头+有效位 */
	double	dPoAmt;								/* 票面金额 */
	int	ret;
	char	cPact_no[21];

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );
	memset( cPact_no, 0x00, sizeof( cPact_no ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0370", cPact_no );		/* 承兑汇票协议编号 */
/***	get_zd_double( "100E", &dPoAmt );		 票面金额 */

	/* 检查所输承兑汇票号码是否带冠字头 
	ret = pub_com_ChkNoteHead1( "111", cPoNo1 );
	if( ret )
	{
		sprintf( acErrMsg, "检查承兑汇票号码出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	 取承兑汇票冠字头 
	strncpy( cPoNoHead, cPoNo1, 8 );
	cPoNoHead[8]='\0';
	pub_base_strpack( cPoNoHead );

	 取承兑汇票有效位 
	strncpy( cPoNo, cPoNo1+8, 8 );
	cPoNo[8] = '\0';
sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
WRITEMSG

	 承兑汇票号码冠字头+有效位 
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
WRITEMSG
	pub_base_strpack_all( cPoNo2 );
	******/
	/* 查询出票人账号，为下面记交易流水赋值用 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", cPact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此承兑协议编号![%s]", sMoBankPo.pact_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B187" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	if(sMoBankAccPo.acc_po_ind[0]=='0')
	{
		sprintf( acErrMsg, "未做到期处理![%d]", ret );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"未做到期处理!!!");
		goto ErrExit;
	}
	pub_base_strpack(cPact_no);
	/*** 修改承兑汇票登记薄,登记收到日期、状态置为托收 **/
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and po_sts not in ('2','3') ", sMoBankAccPo.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		memset(&sMoBankPo,0x00,sizeof(sMoBankPo));
		ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
		if( ret == 100 )
		{
			break;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Mo_bank_po_Fet_Upd错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	
		if( sMoBankPo.po_sts[0] == '4' )
		{
			sprintf( acErrMsg, "此承兑汇票已做托收!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B220" );
			goto ErrExit;
		}else if( sMoBankPo.po_sts[0] == '3' )
		{
			sprintf( acErrMsg, "此承兑汇票已做退票!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B212" );
			goto ErrExit;
		}else if( sMoBankPo.po_sts[0] == '2' )
		{
			sprintf( acErrMsg, "此承兑汇票已核销!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B234" );
			goto ErrExit;
		}
		
		/* 修改承兑汇票基本信息登记簿，状态为托收*/
		strcpy( sMoBankPo.po_sts, "4" );		/* 标志：4 托收 */
		sMoBankPo.receive_date = g_pub_tx.tx_date;	/* 托收日期 */
	
		ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改承兑汇票登记薄错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		/* 记流水日志 */
		/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
		strcpy( g_pub_tx.note_type, "111" );				/* 银行承兑汇票 */
		strcpy(g_pub_tx.ac_no,sMoBankPo.pact_no);
		strcpy(g_pub_tx.beg_note_no,sMoBankPo.po_no);
		g_pub_tx.tx_amt1=0.00;
		strcpy( g_pub_tx.brf, "银行承兑汇票托收登记" );
		ret = pub_ins_trace_log();
		if ( ret )
		{
			sprintf( acErrMsg, "记流水日志错" );
			WRITEMSG
			goto ErrExit;
		}
	}
	Mo_bank_po_Clo_Upd();

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
