/***************************************************************
* 文 件 名: spB312.c
* 功能描述：本票签发子交易
*			更新签发登记簿中的信息
*
* 作    者: jane
* 完成日期：2003年11月08日
*
* 修改记录：
*    日    期: 
*    修 改 人: 
*    修改内容: 
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_parm_c.h"
#include "mo_cashier_c.h"

spB312()
{
	struct mo_cashier_c sMoCashier;
	struct com_parm_c com_parm_v;

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );

	pub_base_sysinit();

	/*检查签发权限*/
	g_reply_int = pub_com_ChkBrRel( 5, g_pub_tx.tx_br_no, g_pub_tx.tx_br_no );
	if( g_reply_int == 6 )
	{
		sprintf( acErrMsg, "本机构无权签发银行本票" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "检查签发银行本票权限出错" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	get_zd_data( "0730", sMoCashier.pact_no );

	/*修改原记录*/
	g_reply_int = Mo_cashier_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and cashier_sts!='*'", 
		sMoCashier.pact_no, sMoCashier.cashier_sts );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "声明查询本票登记簿游标出错[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int = Mo_cashier_Fet_Upd( &sMoCashier, g_pub_tx.reply );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "没有登记该笔业务信息!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "取查询本票登记簿游标出错[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

		if( sMoCashier.cashier_sts[0] == '0' )
		NULL;
		else if(sMoCashier.cashier_sts[0]=='1')
		{
			sprintf( acErrMsg, "已签发本票，本票号码：【%s】", sMoCashier.cashier_no );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else
		{
			sprintf( acErrMsg, "该笔录入信息状态不正常,请联系中心机房![%s]",sMoCashier.cashier_sts );
			MBFEWRITEMSG
			goto ErrExit;
		}

		/*检查该机构该柜员是否有该种类型凭证的这一张*/
		get_zd_data( "0860", sMoCashier.cashier_no );
		g_reply_int = pub_com_ChkNtExt( sMoCashier.cashier_no, sMoCashier.cashier_no, 
			"113", g_pub_tx.tx_br_no, g_pub_tx.tel );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"调用函数检查本票号码错误!");
			WRITEMSG
			goto ErrExit;
		}
		sMoCashier.sign_date = g_pub_tx.tx_date;
		sMoCashier.sign_trace_no = g_pub_tx.trace_no;
		strcpy( sMoCashier.sign_tel, g_pub_tx.tel );
		strcpy( sMoCashier.sign_br_no, g_pub_tx.tx_br_no );
		strcpy( sMoCashier.cashier_sts, "1" );

	sprintf( acErrMsg, "mo_cashier: N pact_no[%s] N tx_br_no[%s] tx_date[%ld] sign_amt[%lf] ct_ind[%s] ration_ind[%s] N requ_ac_no[%s] N requ_name[%s] requ_addr[%s] payee_ac_no[%s] payee_name[%s] rmk[%s] rec_trace_no[%ld] rec_tel[%s] N cashier_sts[%s] N use_ind[%s] N los_rpt_ind[%s]", 
		sMoCashier.pact_no, sMoCashier.tx_br_no, sMoCashier.tx_date, 
		sMoCashier.sign_amt, sMoCashier.ct_ind, sMoCashier.ration_ind, 
		sMoCashier.requ_ac_no, sMoCashier.requ_name, sMoCashier.requ_addr, 
		sMoCashier.payee_ac_no, sMoCashier.payee_name, sMoCashier.rmk, 
		sMoCashier.rec_trace_no, sMoCashier.rec_tel, sMoCashier.cashier_sts, 
		sMoCashier.use_ind, sMoCashier.los_rpt_ind );
	WRITEMSG

	g_reply_int = Mo_cashier_Upd_Upd( sMoCashier, g_pub_tx.reply );
	if( g_reply_int )
    {
        sprintf( acErrMsg, "执行更新本票签发登记簿游标错误![%d]", g_reply_int );
        WRITEMSG
        goto ErrExit;
    }
	Mo_cashier_Clo_Upd();


	if( pub_com_NoteUse( 0, 0, "113", sMoCashier.cashier_no, 
		sMoCashier.cashier_no, g_pub_tx.tel ) )
	{
		sprintf( acErrMsg, "银行本票凭证销号出错!" );
       	MBFEWRITEMSG
		goto ErrExit;
	}

		g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='BPKM' and parm_seqn='1'" );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "查询公共参数表出错[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data( "1201", com_parm_v.val );
		set_zd_data( "120A", "本票签发" );

		memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );
		g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='BPKM' and parm_seqn='2'" );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "查询公共参数表出错[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data( "1211", com_parm_v.val );
		set_zd_data( "121A", "本票签发" );

		set_zd_data( "1206", "113" );    /*凭证类型*/
		set_zd_data( "1207", sMoCashier.cashier_no );    /*凭证号码*/
		set_zd_data( "1216", "113" );    /*凭证类型*/
		set_zd_data( "1217", sMoCashier.cashier_no );    /*凭证号码*/

		/*记交易日志为撤销用*/
		get_zd_data( "0860", sMoCashier.cashier_no );
		strcpy( g_pub_tx.no_show, "1" );/*隐形*/
		strcpy( g_pub_tx.ac_no, sMoCashier.cashier_no );
		strcpy( g_pub_tx.add_ind, "1" );
		strcpy( g_pub_tx.brf, "本票签发" );
		g_reply_int = pub_ins_trace_log();
		if( g_reply_int )
		{
			sprintf( acErrMsg," 记交易日志错误![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"更新本票签发登记簿成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"更新本票签发登记簿失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
