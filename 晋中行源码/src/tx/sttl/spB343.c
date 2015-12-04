/*************************************************
* 文 件 名:  spB343.c
* 功能描述： 本交易用于管理承兑拆分类型的原始票据 
*	    	 信息及登记承兑协议和保证金账户； 
*
* 作    者:  jane
* 完成日期： 2003年8月24日
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
#include "mo_bank_acc_po_c.h"
#include "mo_bank_split_po_c.h"

int spB343()
{
	struct	mo_bank_acc_po_c 	sMoBankAccPo;	/* 承兑汇票基本信息登记簿 */
	struct	mo_bank_split_po_c 	sMoBankSplitPo;	/* 承兑拆分原始票据登记簿 */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* 介质账号对照表 */
	struct	dd_mst_c 			sDdMst;			/* 活期主文件 */
	char	cBailAcNo[25];						/* 保证金帐号 */
	int		ret;

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	strcpy( sMoBankSplitPo.tx_br_no, g_pub_tx.tx_br_no );	/* 交易机构 */
	sMoBankSplitPo.req_date = g_pub_tx.tx_date;				/* 申请日期 */
	get_zd_data( "0620", sMoBankSplitPo.pact_no );			/* 质押编号 */
	get_zd_data( "0830", cBailAcNo );						/* 保证金帐号 */
	get_zd_double( "1002", &sMoBankSplitPo.par_amt );		/* 汇票金额 */
	get_zd_long( "0450", &sMoBankSplitPo.matr_date );		/* 汇票到期日 */
	get_zd_data( "0310", sMoBankSplitPo.payer_ac_no );		/* 出票人帐号 */
	get_zd_data( "0260", sMoBankSplitPo.payer_name );		/* 出票人全称 */
	get_zd_data( "0810", sMoBankSplitPo.payer_br_name );	/* 付款行全称 */
	get_zd_data( "0320", sMoBankSplitPo.payee_ac_no );		/* 收款人帐号 */
	get_zd_data( "0270", sMoBankSplitPo.payee_name );		/* 收款人全称 */
	get_zd_data( "0820", sMoBankSplitPo.payee_br_name );	/* 收款人开户行全称 */
	get_zd_data( "0600", sMoBankSplitPo.pnote_no );			/* 票据号 */

	/* 检查承兑拆分原始票据登记簿中有无此协议编号 */
	ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, "pact_no='%s'", \
						sMoBankSplitPo.pact_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "此质押编号已存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B307" );
		goto ErrExit;
	}else if( ret !=0 && ret != 100 )
	{
		sprintf( acErrMsg, "执行Mo_bank_split_po_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 检查承兑拆分原始票据登记簿中有无此承兑汇票记录 */
	ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, "pnote_no='%s'", \
						sMoBankSplitPo.pnote_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "此承兑汇票已做拆分登记!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B294" );
		goto ErrExit;
	}else if( ret !=0 && ret != 100 )
	{
		sprintf( acErrMsg, "执行Mo_bank_split_po_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 检查保证金帐号的有效性:活期账号、账户状态正常 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
						cBailAcNo );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "保证金账号不存在![%s]", cBailAcNo );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B195" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "输入的保证金账号非活期账号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B196" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( strcmp( sDdMst.prdt_no, "131" ) !=0 )
	{
		sprintf( acErrMsg, "输入的账号非承兑保证金账号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B202" );
		goto ErrExit;
	}

	if( sDdMst.ac_sts[0] != '1' )
	{
		sprintf( acErrMsg, "此保证金账号状态不正常!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B197" );
		goto ErrExit;
	}

	/* 登记承兑拆分原始票据登记簿 */
	strcpy( sMoBankSplitPo.bail_ac_no, cBailAcNo );		/* 保证金帐号 */
	strcpy( sMoBankSplitPo.sts, "1" );       		/* 状态：1 录入 */
	ret = Mo_bank_split_po_Ins( sMoBankSplitPo, g_pub_tx.reply );
	if ( ret )
   	{
       	sprintf( acErrMsg, "执行Mo_bank_split_po_Ins错误!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}

	/* 登记汇票登记薄 */
	strcpy( sMoBankAccPo.bail_ac_no, cBailAcNo );			/* 保证金帐号 */

	ret = pub_sttl_RegMoBankAccPo( sMoBankAccPo );
	if ( ret )
   	{
   		sprintf( acErrMsg, "执行pub_sttl_RegMoBankAccPo错误!ret=[%d]", ret );
   		WRITEMSG
   		goto ErrExit;
   	}

	/* 为记流水作准备 */
	strcpy( g_pub_tx.brf, "承兑汇票拆分票据管理" );	/* 备注 */

	/* 记流水日志 */
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoBankSplitPo.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");
	strcpy(g_pub_tx.note_type,"111");	/*** 承兑汇票类型 ***/
	strcpy( g_pub_tx.beg_note_no, cBailAcNo );	/*** 保证金帐号撤销用 ***/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "0820", sDdMst.name );    /* 为打印传户名 */

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
