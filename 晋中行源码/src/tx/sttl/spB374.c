/*************************************************
* 文 件 名:  spB374.c
* 功能描述： 完成银行承兑汇票质押存单出库功能
*
* 作    者:  
* 完成日期： 2003年11月07日
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

int spB374()
{
	struct	impa_dep_reg_c 		sImpaDepReg;	/* 质押存单登记簿 */
	struct	mo_hold_c			sMoHold;
	struct  mdm_ac_rel_c		sMdmAcRel;
	char    cLoadName[50];       		   		/* 上传文件名 */
	double	sum_je=0.00;
	int		ret=0;

	/* 数据初始化 */
	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMoHold, 0x00 ,sizeof( struct mo_hold_c ) );
	memset( &sMdmAcRel, 0x00 ,sizeof( struct mdm_ac_rel_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0630", sImpaDepReg.pact_no );			/* 质押编号 */

	/* 检查质押存单登记薄中是否已存在此质押协议编号 */
	ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, " pact_no='%s' ", sImpaDepReg.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "执行Impa_dep_reg_Sel出错![%d][%s]", ret,sImpaDepReg.pact_no );
		WRITEMSG
		goto ErrExit;
	}

	/* 检查质押总金额是否相符 */
	ret = sql_sum_double("impa_dep_reg","mort_amt",&sum_je," pact_no='%s' ",sImpaDepReg.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "计算总金额错误![%d][%s]", ret,sImpaDepReg.pact_no );
		WRITEMSG
		goto ErrExit;
	}
	get_zd_double( "1002", &sImpaDepReg.mort_amt );			/* 质押总金额 */
	if( pub_base_CompDblVal(sum_je,sImpaDepReg.mort_amt)!=0 )
	{
		sprintf( acErrMsg, "总金额错误![%.2f][%.2f][%s]",sum_je,sImpaDepReg.mort_amt,sImpaDepReg.pact_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B186");
		goto ErrExit;
	}

	ret = Impa_dep_reg_Dec_Upd( g_pub_tx.reply, " pact_no='%s' ", sImpaDepReg.pact_no );
	if( ret )
	{
		sprintf(acErrMsg,"执行Impa_dep_reg_Dec_Upd错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Impa_dep_reg_Fet_Upd( &sImpaDepReg, g_pub_tx.reply );
		if( ret == 100 ) break;
		else if( ret )
		{
			sprintf( acErrMsg, "执行Impa_dep_reg_Fet_Upd出错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

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

		ret = Td_mst_Fet_Upd( &g_td_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "质押存单号不存在![%s]", sImpaDepReg.mort_ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B183" );
			goto ErrExit;
		}else if ( ret )
		{
			sprintf( acErrMsg, "执行Td_mst_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		if( g_td_mst.hold_sts[0]!='1' )
		{
			sprintf( acErrMsg, "没有被冻结[%d][%ld]",ret,g_td_mst.ac_id );
			WRITEMSG
			continue;
		}
		strcpy( g_td_mst.hold_sts, "0" );		/* 冻结状态: 0 正常 */
		g_td_mst.hold_amt=0.00;
TRACE
		ret = Td_mst_Upd_Upd( g_td_mst, g_pub_tx.reply );
    		if ( ret )
		{
			sprintf( acErrMsg, "执行Td_mst_Upd_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
		Td_mst_Clo_Upd( );

TRACE
		/* 修改冻结登记簿 */
		ret = Mo_hold_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and hold_sts='0' ", sMdmAcRel.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_hold_Dec_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset(&sMoHold,0x00,sizeof(struct mo_hold_c));
			ret = Mo_hold_Fet_Upd( &sMoHold, g_pub_tx.reply );
			if ( ret==100 )	break;
			else if ( ret )
			{
				sprintf( acErrMsg, "执行Mo_hold_Fet_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( sMoHold.hold_sts, "1" );				/* 冻结状态解除: 1 解除*/
			sMoHold.fct_unhold_date = g_pub_tx.tx_date;		/* 实际解冻日期 */
			strcpy( sMoHold.unhold_br_no,g_pub_tx.tx_br_no);/* 解冻机构号*/
			strcpy( sMoHold.unhold_tel, g_pub_tx.tel);		/* 解冻操作员*/
			sMoHold.wrk_date = g_pub_tx.tx_date;			/* 交易日期 */
			sMoHold.trace_no = g_pub_tx.trace_no;			/* 流水号 */
			strcpy( sMoHold.unhold_brf,"承兑汇票质押存单出库" );

			/**
			pub_base_strpack(sMoHold.hold_brf);
			if( !strcmp(sMoHold.hold_brf,"签发承兑汇票自动冻结") 
				&& strcmp(sMoHold.hold_br_no,g_pub_tx.tx_br_no) )
			{
				sprintf( acErrMsg, "非该机构做的质押存单录入[%s][%s]",sMoHold.hold_br_no,g_pub_tx.tx_br_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"B344");
				goto ErrExit;
			}
			**/
			ret = Mo_hold_Upd_Upd( sMoHold, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "执行Mo_hold_Upd_Upd错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
TRACE
		}
		Mo_hold_Clo_Upd( );
		ret = Impa_dep_reg_Del_Upd(sImpaDepReg,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "执行Impa_dep_reg_Del_Upd出错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

	}
	Impa_dep_reg_Clo_Sel ();

	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "承兑汇票质押存单出库" );	/* 备注 */
	/* 记流水日志 */
	if(strlen(sImpaDepReg.pact_no)>19)
	{
		memcpy(g_pub_tx.ac_no,sImpaDepReg.pact_no+1,sizeof(g_pub_tx.ac_no)-1);
	}else
	{
		memcpy(g_pub_tx.ac_no,sImpaDepReg.pact_no,sizeof(g_pub_tx.ac_no)-1);
	}
	memcpy(g_pub_tx.beg_note_no,sImpaDepReg.pact_no,16);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0370",sImpaDepReg.pact_no);/*** 修改抵质押登记薄 add by chenchao 20110806 **/
	set_zd_double("0390",sum_je);		/*** 修改抵质押登记薄 add by chenchao 20110806 **/

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
