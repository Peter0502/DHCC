/*************************************************
* 文 件 名:  spB330.c
* 功能描述： 本行企业拿他行的承兑汇票来抵押,对他行的承兑汇票上的内容进行登记 对应前台5621
*
* 作    者:  liuxuan
* 完成日期： 2006年10月7日
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
#include "mo_bank_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_dep_reg_c.h"
#include "mo_bank_split_po_c.h"
#include "mo_po_co_c.h"
#include "mo_bail_rate_c.h"

int spB330()
{
	struct	mo_bank_acc_po_c 	sMoBankAcc,sMoBankAcc1;		/* 承兑汇票基本信息登记簿 */
	struct	mo_bank_po_c 	sMoBankPo,sMoBankPo_c;	/* 承兑汇票登记簿 */
	struct	mo_sg_cash_po_c 	sMoSgCashPo;	/* 保兑仓登记薄 */
	struct  mo_bank_split_po_c  sMoBankSplitPo; /* 承兑拆分原始票据登记簿 */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* 介质账号对照表 */
	struct	dd_mst_c 			sDdMst;			/* 活期主文件 */
	struct	impa_dep_reg_c 		sImpaDepReg;	/* 质押存单登记簿 */
	struct	mo_dep_reg_c 		sMoDepReg;	/* 汇票质押存单登记簿 */
	struct  mo_po_co_c  sMoPoCo; 	/* 承兑协议编号与合同号对照表 */
	struct  mo_bail_rate_c		mo_bail_rate; 	/* 保证金利率登记薄 */
	char    cLoadName[50];       		   		/* 上传文件名 */
	char	cBailAcNo[25];						/* 保证金帐号 */
	double	dTmpAmt;							/* 保证金金额 */
	double	dTallAmt;							/* 承兑汇票最高限额 */
	double	dSumMortAmt;						/* 质押存单登记簿中质押编号对应余额 */
	double	dSumMortAmt1;						/* 承兑基本信息登记簿中质押编号对应余额之和 */
	double	dSumMortAmt2;						/* 承兑登记簿中质押编号对应余额之和 */
	int		ret, ret1, flag, num;
	char	Tmp_flag[2];

	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankAcc1, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankPo_c, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0330", sMoBankAcc.pact_no );	/*  承兑汇票号也作为协议编号 */
	vtcp_log("协议号是[%s]\n",sMoBankAcc.pact_no );

	pub_base_strpack( sMoBankAcc.pact_no );

	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret1 = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", \
						sMoBankAcc.pact_no );
	if ( ret1 )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret1 );
		WRITEMSG
		goto ErrExit;
	}

	ret1 = Mo_bank_acc_po_Fet_Upd( &sMoBankAcc, g_pub_tx.reply );
	vtcp_log("fet的结果ret1是 [%d]\n",ret1);
	if( ret1 && ret1 != 100 )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret1 );
		WRITEMSG
		goto ErrExit;
	}
		/* 从前台屏幕取值 */
		strcpy( sMoBankAcc.tx_br_no, g_pub_tx.tx_br_no );	/* 交易机构 */
		get_zd_long( "0450", &sMoBankAcc.req_date);			/* 申请日期 在这里作为起息日 */
		get_zd_data( "0260", sMoBankAcc.payer_name );		/* 付款人全称 */
		get_zd_data( "0250", sMoBankAcc.payer_br_name );	/* 付款人开户行行名 */
		get_zd_data( "0270", sMoBankAcc.payee_name );		/* 收款人全称 */
		get_zd_data( "0830", sMoBankAcc.payee_ac_no );		/* 收款人账号 */
		get_zd_data( "0810", sMoBankAcc.payee_br_name );	/* 收款人开户行行名 */
		get_zd_double( "1001", &sMoBankAcc.sign_amt );		/* 签发票面金额 */
		vtcp_log("票面金额[%.0f]\n",sMoBankAcc.sign_amt);
		get_zd_long( "0440", &sMoBankAcc.matr_date );		/* 到期日期 */
		get_zd_double( "1002", &sMoBankAcc.tot_mort_amt );	/* 质押金额 */
		get_zd_double( "0400", &sMoBankAcc.bail_amt_ratio );/* 保证金比例 */
		strcpy( sMoBankAcc.acc_po_ind, "0" );				/* 汇票状态: 0未到期处理*/
		get_zd_data( "0380", sMoBankAcc.payer_ac_no );		/* 付款人帐号 */
		get_zd_double( "1003", &dTmpAmt );					/* 保证金金额 */
		get_zd_data( "0670", sMoBankAcc.acc_type );			/* 承兑类型 */
		get_zd_data( "0320", cBailAcNo );					/* 在本交易里作为本行企业帐号 */
		strcpy( sMoBankAcc.acc_po_sts, "9" );				/* 用来表示外行过来的质押票 */

		if( ret1 == 100 )
		{
			vtcp_log("执行的是insert\n");
			ret = Mo_bank_acc_po_Ins( sMoBankAcc, g_pub_tx.reply );
			if( ret ){
				sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		else{
			vtcp_log("执行的是update\n");
			/* 修改承兑汇票基本信息登记簿 */
			ret = Mo_bank_acc_po_Upd_Upd( sMoBankAcc, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}

	Mo_bank_acc_po_Clo_Upd();

	strcpy( sMoBankPo.sign_br_no, sMoBankAcc.payer_br_no );  /* 签发行行号 */
	strcpy( sMoBankPo.pact_no, sMoBankAcc.pact_no);        /* 协议编号 */
	memcpy( sMoBankPo.po_no, sMoBankAcc.pact_no,sizeof(sMoBankPo.po_no)-1);        /* 承兑汇票号*/
	sMoBankPo.sign_date = sMoBankAcc.req_date;             /* 签发日期 */
	sMoBankPo.receive_date = sMoBankAcc.req_date;             /* 签发日期 */
	sMoBankPo.po_amt = sMoBankAcc.sign_amt;             /* 票面金额 */
	strcpy( sMoBankPo.at_term_ind, "N" );               /* 票据到期标志:Y 到期；N 未到期 */
	strcpy( sMoBankPo.po_sts, "9" );                    /* 承兑汇票状态:9 外行的票 */
	strcpy( sMoBankPo.po_ind, "1" );                    /* 承兑汇票标志:1 正常 */

	/* 根据承兑汇票号码查找银行承兑汇票登记薄 */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo_c, "po_no='%s'", sMoBankPo.po_no);
	if(ret && ret!=100){
		sprintf( acErrMsg, "执行Mo_bank_po_Sel错[%d]", ret1 );
		WRITEMSG
		goto ErrExit;
	}
	if( ret == 0 )
	{
		vtcp_log( "[%s][%d]承兑汇票号码已存在,做更新处理");
		/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
		ret1 = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and po_no='%s'", \
								sMoBankPo.pact_no ,sMoBankPo.pact_no);
			if ( ret1 )
			{
				sprintf( acErrMsg, "执行Mo_bank_po_Dec_Upd错[%d]", ret1 );
				WRITEMSG
				goto ErrExit;
			}
			ret1 = Mo_bank_po_Fet_Upd( &sMoBankPo_c, g_pub_tx.reply );
			vtcp_log("fet的结果ret1是 [%d]\n",ret1);
			if( ret1)
			{
				sprintf( acErrMsg, "执行Mo_bank_po_Sel错![%d]", ret1 );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( sMoBankPo.rowid, sMoBankPo_c.rowid );	/* 用上面赋值过的更新 */
			ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "修改承兑汇票登记薄错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			Mo_bank_po_Clo_Upd();
		}
	else{
			vtcp_log("无此承兑汇票[%s],进行插入操作\n",sMoBankPo.pact_no);
			ret = Mo_bank_po_Ins( sMoBankPo, g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "执行Mo_bank_po_Ins错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
	}

	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "质押承兑汇票信息录入" );	/* 备注 */

	/* 记流水日志 */
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAcc.pact_no);
	g_pub_tx.tx_amt1=0.00;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

	/*登记协议编号-合同号对照登记簿 为MIS准备数据*/
	memset( &sMoPoCo, 0x00, sizeof( struct mo_po_co_c ) );
	strcpy( sMoPoCo.pact_no, sMoBankAcc.pact_no );
	strcpy( sMoPoCo.contract_no, sMoBankAcc.pact_no );
	g_reply_int = Mo_po_co_Ins( sMoPoCo,g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "登记承兑协议编号-合同号对照表出错" );
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
