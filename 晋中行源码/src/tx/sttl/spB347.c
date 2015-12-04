/*************************************************
* 文 件 名:  spB347.c
* 功能描述： 承兑汇票查询书管理
*
* 作    者:  xxx
* 完成日期： 2003年9月25日
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
#include "mo_bank_po_query_c.h"

int spB347()
{
	struct	mo_bank_po_c		sMoBankPo;		/* 承兑汇票登记簿 */
	struct	mo_bank_po_query_c	sMoBankPoQuery;	/* 承兑汇票查询书登记簿 */
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* 承兑汇票基本信息登记簿 */
	char	cPoNo1[17];							/* 承兑汇票号码屏幕取值 */
	char	cPoNo[9];							/* 承兑汇票号码有效位 */
	char	cPoNoHead[9];						/* 承兑汇票号码冠字头 */
	char	cPoNo2[17];							/* 承兑汇票号码冠字头+有效位 */
	char	po_ind[2];							/* 操作状态：1-挂失2-解挂3-冻结4-解冻 */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankPoQuery, 0x00, sizeof( struct mo_bank_po_query_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0580", cPoNo1 );		/* 承兑汇票号码 */
	get_zd_long( "0450", &sMoBankPoQuery.query_date);
	get_zd_data( "0590", sMoBankPoQuery.query_br_no);
	get_zd_data( "0820", sMoBankPoQuery.query_brf);

	/* 检查所输承兑汇票号码是否带冠字头 */
	ret = pub_com_ChkNoteHead1( "111", cPoNo1 );
	if( ret )
	{
		sprintf( acErrMsg, "检查承兑汇票号码出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 取承兑汇票冠字头 */
	strncpy( cPoNoHead, cPoNo1, 8 );
	cPoNoHead[8]='\0';
	pub_base_strpack( cPoNoHead );

	/* 取承兑汇票有效位 */
	strncpy( cPoNo, cPoNo1+8, 8 );
	cPoNo[8] = '\0';
sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
WRITEMSG

	/* 承兑汇票号码冠字头+有效位 */
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
WRITEMSG
	pub_base_strpack_all( cPoNo2 );

	/* 修改承兑汇票登记薄,登记收到日期、状态置为托收 */
	ret = Mo_bank_po_Sel( g_pub_tx.reply,&sMoBankPo,"po_no='%s'", cPoNo2 );
	if ( ret )
	{
		sprintf( acErrMsg, "无此承兑汇票!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B217" );
		goto ErrExit;
	}

	if( sMoBankPo.po_sts[0] == '3' )
	{
		sprintf( acErrMsg, "此承兑汇票已做退票!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B212" );
		goto ErrExit;
	}
	else if( sMoBankPo.po_sts[0] == '2' )
	{
		sprintf( acErrMsg, "此承兑汇票已核销!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B234" );
		goto ErrExit;
	}

	/* 查询出票人账号，为下面记交易流水赋值用 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoBankPo.pact_no );
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

	/* 登记承兑汇票查询书登记薄 */
	sMoBankPoQuery.tx_date=g_pub_tx.tx_date;
	sMoBankPoQuery.trace_no=g_pub_tx.trace_no;
	strcpy(sMoBankPoQuery.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sMoBankPoQuery.po_no,cPoNo2);
	strcpy(sMoBankPoQuery.sts,"1");		/** 正常 **/
	ret=Mo_bank_po_query_Ins(sMoBankPoQuery,g_pub_tx.reply);
	if ( ret )
	{
		sprintf( acErrMsg, "登记承兑汇票查询书错误![%d]",ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B323" );
		goto ErrExit;
	}

	/* 记流水日志 */
	strcpy(g_pub_tx.ac_no,sMoBankPo.pact_no);
	strcpy(g_pub_tx.beg_note_no,cPoNo2);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.brf,"承兑汇票查询书登记");
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
