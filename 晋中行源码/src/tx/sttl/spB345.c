/*************************************************
* 文 件 名:  spB345.c
* 功能描述： 完成银行承兑汇票质押存单录入功能
*
* 作    者:  jack
* 完成日期： 2005年8月31日
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
#include "impa_dep_reg_c.h"
#include "mo_hold_c.h"

int spB345()
{
	struct	impa_dep_reg_c 		sImpaDepReg;	/* 质押存单登记簿 */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* 介质账号对照表 */
	struct	td_mst_c 		sTdMst;			/* 活期主文件 */
	struct	mo_hold_c 		sMoHold;		/* 冻结登记簿 */
	char    cLoadName[50];       		   		/* 上传文件名 */
	long	lHoldSeqn;					/* 冻结序号 */
	int		ret, flag, num;
	double	dAmt_tot = 0.00;	/**总的质押金额 add by chenchao **/
	double	dIn_amt=0;		/** 入库金额  **/

	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sTdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sMoHold, 0x00, sizeof( struct mo_hold_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0630", sImpaDepReg.pact_no );		/* 质押编号 */
	get_zd_double( "0420", &dIn_amt);

	/* 检查质押存单登记薄中是否已存在此质押协议编号 */
	ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, " pact_no='%s' ", sImpaDepReg.pact_no );
	if( ret == 0 )
	{
		/* 领导要求不控制次数 wudawei 20150914
		sprintf( acErrMsg, "此质押编号已存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B307" );
		goto ErrExit;
		*/
	}
	else if( ret != 100 )
	{
		sprintf( acErrMsg, "执行Impa_dep_reg_Sel出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 从文件导入临时表impa_dep_reg中 */
	if( strcmp( sImpaDepReg.pact_no, "" ) )
	{
		pub_base_GenDwnFilName( cLoadName );
		vtcp_log("[%s][%d] cLoadName=[%s]",__FILE__,__LINE__,cLoadName);
		ret = tab_load( cLoadName, "Impa_dep_reg", "pact_no, mort_ac_no, mort_amt" );
		if( ret )
		{
			sprintf(acErrMsg,"RET [%d]",ret);
			WRITEMSG
       		goto ErrExit;
		}

		/* 检查存单不允许质押多次 */
		/* 目前控制一张存单只能用于一个质押编号的录入 */
		ret = Impa_dep_reg_Dec_Sel( g_pub_tx.reply, " pact_no='%s' ", sImpaDepReg.pact_no );
		if( ret )
		{
			sprintf(acErrMsg,"执行Impa_dep_reg_Dec_Sel错误![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		flag=0;
		while(1)
		{
			ret = Impa_dep_reg_Fet_Sel( &sImpaDepReg, g_pub_tx.reply );
			if( ret == 100 && flag == 0 )
			{
				sprintf( acErrMsg, "此质押编号不存在![%s]", sImpaDepReg.pact_no );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B308" );
				goto ErrExit;
			}
			else if( ret == 100 && flag > 0 )
			{
				break;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Impa_dep_reg_Sel出错![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			dAmt_tot += sImpaDepReg.mort_amt;	/** 累加存单金额 **/
			num = 0;
			/* 计算质押存单出现次数 */
			num = sql_count( "impa_dep_reg", " mort_ac_no='%s' ", sImpaDepReg.mort_ac_no );
			if( num <= 0 )
			{
				sprintf( acErrMsg, "查询质押存单登记簿impa_dep_reg错误!" );
				WRITEMSG
				goto ErrExit;
			}
			else if( num > 1 )
			{
				sprintf( acErrMsg, "一张存单只允许对应一个质押编号!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B309" );
				goto ErrExit;
			}
			flag++;
/**** 晋中商行不进行自动冻结操作  del by chenchao 2011-8-9 16:15:25***/
if(0)
{
			/* 根据质押存单号修改定期主文件 */
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sImpaDepReg.mort_ac_no );
			if( ret == 100 )
			{
				sprintf( acErrMsg, "质押存单号不存在![%s]", sImpaDepReg.mort_ac_no );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B183" );
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			ret = Td_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld", sMdmAcRel.ac_id );
			if ( ret )
			{
				sprintf( acErrMsg, "执行Td_mst_Dec_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

TRACE
			ret = Td_mst_Fet_Upd( &sTdMst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				sprintf( acErrMsg, "质押存单号不存在![%s]", sImpaDepReg.mort_ac_no );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B183" );
				goto ErrExit;
			}
			else if ( ret )
			{
				sprintf( acErrMsg, "执行Td_mst_Fet_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			if( sTdMst.hold_sts[0]=='1' )
			{
				sprintf( acErrMsg, "已经冻结过了，不用再冻结了[%d][%ld]",ret,sTdMst.ac_id );
				WRITEMSG
				continue;
			}
			strcpy( sTdMst.hold_sts, "1" );   	/*	 冻结状态: 1 完全冻结*/

TRACE
			ret = Td_mst_Upd_Upd( sTdMst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "执行Td_mst_Upd_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
			Td_mst_Clo_Upd( );

			/* 查询冻结登记簿中最大冻结序号*/
			ret = sql_max_long( "mo_hold", "hold_seqn", &lHoldSeqn , "ac_id=%ld ",sTdMst.ac_id );
			if( ret )
			{
				sprintf( acErrMsg, "执行sql_max_long错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			/* 登记冻结登记簿 */
			memset(&sMoHold,0x00,sizeof(struct mo_hold_c));
			sMoHold.hold_seqn = lHoldSeqn + 1;	              /*   冻结序号     */
			sMoHold.ac_id = sMdmAcRel.ac_id;	                /*   冻结账户ID */
			sMoHold.ac_seqn = sMdmAcRel.ac_seqn;	            /*   冻结账户序号 */
			strcpy( sMoHold.hold_typ, "1" );	                /*   冻结状态: 1 全部冻结*/
			sMoHold.plan_hold_amt = 0.00;		                  /*   计划冻结金额 */
			strcpy( sMoHold.auto_unhold_ind, "1" );	          /*   自动解冻标志: 1 不自动解冻*/
			strcpy( sMoHold.hold_sts, "0" );	                /*   冻结状态解除: 0 未解除*/
			sMoHold.beg_hold_date = g_pub_tx.tx_date;         /*   冻结起始日期 */
			sMoHold.plan_unhold_date = 0;		                  /*   计划解冻日期 */
			sMoHold.fct_unhold_date = 0;		                  /*   实际解冻日期 */
			strcpy( sMoHold.hold_br_no, g_pub_tx.tx_br_no );  /*   冻结机构号 */
			strcpy( sMoHold.hold_tel, g_pub_tx.tel );         /*   冻结操作员*/
			strcpy( sMoHold.unhold_br_no, "0" );	            /*   解冻机构号*/
			strcpy( sMoHold.unhold_tel, "0" );	              /*   解冻操作员*/
			sMoHold.wrk_date = g_pub_tx.tx_date;	            /*   交易日期 */
			sMoHold.trace_no = g_pub_tx.trace_no;	            /*   流水号 */
			strcpy( sMoHold.hold_brf,"签发承兑汇票自动冻结"); /*          */
		TRACE
			ret = Mo_hold_Ins( sMoHold, g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "执行Mo_hold_Ins错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
}
/**** 晋中商行不进行自动冻结操作  del by chenchao 2011-8-9 16:15:25***/
		Impa_dep_reg_Clo_Sel ();
	}

	vtcp_log("[%s][%d]pact_no=[%s],amt=[%lf]",__FILE__,__LINE__,sImpaDepReg.pact_no,dAmt_tot);
	set_zd_data("0370",sImpaDepReg.pact_no);/** 为更新抵质押登记薄准备 ***/
	set_zd_double("0420",dIn_amt);		/** 为更新抵质押登记薄准备 ***/

	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "承兑汇票质押存单录入" );	/* 备注 */
	/* 记流水日志 */
	/**----- hao ---- @20050819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sImpaDepReg.pact_no);
	memcpy(g_pub_tx.beg_note_no,sImpaDepReg.pact_no,16);
	g_pub_tx.tx_amt1=dIn_amt;
	strcpy(g_pub_tx.add_ind,"");	/**/
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
