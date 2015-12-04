/*************************************************
* 文 件 名:  spB335.c
* 功能描述： 如果付款账号被冻结或者付款人将钱直接划
*	     入保证金账户而不通过结算户，在汇票到期
*	     前须作此交易；保兑仓缴存保证金时也做此
*	     交易;
*
* 作    者:  jane
* 完成日期： 2003年5月28日
*
* 修改记录： 
*   日   期: 2011-8-10 13:18:14
*   修 改 人:chenchao
*   修改内容:晋中要求:在利率维护登记相关信息,因为保证金存款有可能还没有生成承兑协议编号

*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bail_rate_c.h"
#include "mo_sg_cash_po_c.h"
#include "dd_mst_c.h"

int spB335()
{
	struct	mo_bank_acc_po_c 	sMoBankAccPo;	/* 承兑汇票基本信息登记簿 */
	struct	mo_bail_rate_c 		sMo_bail_rate;	/* 承兑汇票基本信息登记簿 */
	struct	mo_sg_cash_po_c 	sMoSgCashPo;	/* 保兑仓登记薄 */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* 介质账号对照表 */
	struct	dd_mst_c		sDd_mst;
	struct	dd_mst_c 			sDdMst;			/* 活期主文件 */
	double	dAmt;								/* 补缴金额 */
	char	cOperCode[2];						/* 操作码 */
	char	cAmtSource[2];						/* 金额来源 */
	char	cAcNo[20];							/* 扣款账号 */
	char	cBailAcNo[25];						/* 保证金帐号 */
	int	ret;
	char	cPact_no[21];
	

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sMo_bail_rate, 0x00, sizeof( sMo_bail_rate ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sDd_mst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cAmtSource, 0x00, sizeof( cAmtSource ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );
	memset( cPact_no, 0x00, sizeof( cPact_no ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0680", cOperCode );			/* 操作码 */
	get_zd_data( "0330", sMoBankAccPo.pact_no );		/* 承兑汇票协议编号 */
	get_zd_double( "1004", &dAmt );				/* 补缴金额 */
	get_zd_data( "0670", cAmtSource );			/* 金额来源 */
	get_zd_data( "0310", cAcNo );				/* 扣款账号 */
	get_zd_data( "0320", cBailAcNo );			/* 保证金帐号 */

	/** 旧帐号处理 **/
	pub_base_old_acno( cAcNo );
	pub_base_old_acno( cBailAcNo );
	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s')",cBailAcNo);
	if( ret )
	{
		sprintf( acErrMsg, "执行Dd_mst_Sel错![%d][%s]",ret,cBailAcNo);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(memcmp(sDd_mst.prdt_no,"131",3) && memcmp(sDd_mst.prdt_no,"142",3) )
	{
		sprintf( acErrMsg, "非保证金账户不允许用此交易![%d][%s]",ret,cBailAcNo);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if( cOperCode[0] == '2' )			/* 补缴保证金 */
	{
		/* 将补缴金额加到承兑汇票基本信息登记簿中保证金额一项中 */
		ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankAccPo.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	
		ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
		if( ret && ret!=100)
		{
			sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Upd错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}else if(ret == 0)
		{
			if( sMoBankAccPo.acc_po_ind[0] == '1' )
			{
				sprintf( acErrMsg, "此承兑已到期!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B263" );
				goto ErrExit;
			}

			/* xxx ---- 20031107 ---- 检查保证金帐号是否正确 ----*/
			pub_base_strpack(sMoBankAccPo.bail_ac_no);
			sprintf( acErrMsg, "保证金帐号[%s][%s]",sMoBankAccPo.bail_ac_no,cBailAcNo);
			WRITEMSG
			if( strlen(sMoBankAccPo.bail_ac_no)>0 && strcmp(sMoBankAccPo.bail_ac_no,cBailAcNo) )
			{
	   			sprintf( acErrMsg, "保证金帐号错误![%s][%s]",sMoBankAccPo.bail_ac_no,cBailAcNo);
				WRITEMSG
				strcpy(g_pub_tx.reply,"B258");
				goto ErrExit;
			}
			if( strlen(sMoBankAccPo.bail_ac_no)==0 )
			{
				strcpy(sMoBankAccPo.bail_ac_no,cBailAcNo);
			}
			/**------------- xxx -------------- end -----------------**/

			/* 修改承兑汇票基本信息登记簿 */
			sMoBankAccPo.bail_amt += dAmt;		/* 保证金额 */
			sMoBankAccPo.bail_amt_ratio = pub_base_PubDround_1(sMoBankAccPo.bail_amt*100/sMoBankAccPo.sign_amt);/** 更新保证金比例***/
			ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
                	
			Mo_bank_acc_po_Clo_Upd();
                	
			/* 如果承兑类型为保兑仓，还需修改保兑仓登记簿 */
			ret = Mo_sg_cash_po_Dec_Upd( g_pub_tx.reply, "pact_no = '%s'", sMoBankAccPo.pact_no );
			if( ret )
			{
				sprintf( acErrMsg, "执行Mo_sg_cash_po_Dec_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
                	
			ret = Mo_sg_cash_po_Fet_Upd( &sMoSgCashPo, g_pub_tx.reply );
			if ( ret == 100 )
			{
				sprintf( acErrMsg, "此承兑非保兑仓!" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B267" );
				goto ErrExit;
			}else if ( ret )
			{
				sprintf( acErrMsg, "执行Mo_sg_cash_po_Fet_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
			sMoSgCashPo.safeg_sum += dAmt;		/* 已存保证金金额 */
			ret = Mo_sg_cash_po_Upd_Upd( sMoSgCashPo , g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "修改保兑仓登记薄错[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
                	
			Mo_sg_cash_po_Clo_Upd( );
			/* 如果承兑类型为利率登记薄 */
			ret =Mo_bail_rate_Dec_Upd( g_pub_tx.reply, "pact_no = '%s'", sMoBankAccPo.pact_no );
			if( ret )
			{
				sprintf( acErrMsg, "执行Mo_bail_rate_Dec_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
                	
			ret = Mo_bail_rate_Fet_Upd( &sMo_bail_rate, g_pub_tx.reply );
			if ( ret && ret!=100)
			{
				sprintf( acErrMsg, "执行Mo_sg_cash_po_Fet_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
			sMo_bail_rate.amt += dAmt;		/* 已存保证金金额 */
			ret = Mo_bail_rate_Upd_Upd( sMo_bail_rate , g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "修改保兑仓登记薄错[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
                	
			Mo_bail_rate_Clo_Upd();
		}else{
			sprintf( acErrMsg, "查询失败[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
		
			
		/* 为记流水作准备 */
		strcpy( g_pub_tx.brf, "承兑汇票补缴保证金" );	/* 备注 */
	}

	/* 记流水日志 */
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no);
	g_pub_tx.tx_amt1=dAmt;
	strcpy(g_pub_tx.no_show,"1");
	strcpy(g_pub_tx.add_ind,cOperCode);			/** 缴存补缴 **/
	strcpy(g_pub_tx.note_type,sMoBankAccPo.acc_type);	/*** 承兑汇票类型 ***/
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
