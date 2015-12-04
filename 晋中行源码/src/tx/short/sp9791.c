/*************************************************************
* 文 件 名:  sp9791.c
* 功能描述： 大额支付快捷查询
*			 汇票签发根据业务编号反显录入信息
*
* 作    者:  jane
* 完成日期： 2003年6月8日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_branch_c.h"

int sp9791()
{
	char oprtflg[2];
	struct mbfebillopnreg_c mbfebillopnreg_v;
	struct com_branch_c com_branch_v;

	memset( &mbfebillopnreg_v, 0x00, sizeof( struct mbfebillopnreg_c ) );
	memset( &com_branch_v, 0x00, sizeof( struct com_branch_c ) );
	memset( &mbfetran, 0x00, sizeof( struct MBFETRAN ) );

	pub_base_sysinit();

	if( get_bkno() ) goto ErrExit;

	get_zd_data( "0730", mbfebillopnreg_v.pact_no );
	get_zd_data( "0700", oprtflg );

	g_reply_int = Mbfebillopnreg_Sel( g_pub_tx.reply, &mbfebillopnreg_v, 
		"pact_no='%s' and billsts!='*'", mbfebillopnreg_v.pact_no );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "没有登记该笔业务信息!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "查询汇票登记簿数据库操作出错[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( oprtflg[0] == '1' )	/*签发*/
	{
		if( mbfebillopnreg_v.billsts[0] == '0' )
		;
		else if(mbfebillopnreg_v.billsts[0]=='1')
		{
			sprintf( acErrMsg, "已签发汇票，汇票号码：【%s】", mbfebillopnreg_v.billcode );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if(mbfebillopnreg_v.billsts[0]=='2')
		{
			sprintf( acErrMsg, "已签发并移存汇票，汇票号码：【%s】", mbfebillopnreg_v.billcode );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else
		{
			sprintf( acErrMsg, "该笔录入信息状态不正常,请联系中心机房![%s]",mbfebillopnreg_v.billsts );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	else if( oprtflg[0] == '2' || oprtflg[0] == '3' || oprtflg[0] == '4' )    /*重打/重签/撤签*/
	{
		if( mbfebillopnreg_v.billsts[0] == '0' )
		{
			sprintf( acErrMsg, "该笔业务信息尚未签发汇票" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfebillopnreg_v.billsts[0] == '2' )
		{
			sprintf( acErrMsg, "汇票已经移存，汇票号码：【%s】", mbfebillopnreg_v.billcode );
			MBFEWRITEMSG
			goto ErrExit;
		}

		if( strcmp( mbfebillopnreg_v.payeropenaccbkcode, mbfetran.cnapsodfi ) )
		{
        	sprintf( acErrMsg, "非本行签发汇票!");
        	MBFEWRITEMSG
			goto ErrExit;
		}

		if( oprtflg[0] == '3' )
		{
			memset( mbfebillopnreg_v.billcode, 0x00, sizeof( mbfebillopnreg_v.billcode ) );
			memset( mbfebillopnreg_v.billofxchngseal, 0x00, sizeof( mbfebillopnreg_v.billofxchngseal ) );
		}
	}

	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &com_branch_v, 
		"br_no='%s'", mbfebillopnreg_v.tx_br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询录入机构名称出错![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0860", mbfebillopnreg_v.billcode );
	set_zd_data( "0880", mbfebillopnreg_v.billofxchngseal );
	set_zd_data( "0910", mbfebillopnreg_v.tx_br_no );
	set_zd_data( "0620", com_branch_v.br_name );
	set_zd_data( "0630", mbfebillopnreg_v.brf );
	set_zd_long( "0460", mbfebillopnreg_v.tx_date );
	set_zd_double( "0390", mbfebillopnreg_v.amount );
	set_zd_data( "0210", mbfebillopnreg_v.billtype );
	set_zd_data( "0300", mbfebillopnreg_v.payeracc );
	set_zd_data( "0250", mbfebillopnreg_v.payername );
	set_zd_data( "0260", mbfebillopnreg_v.payeraddr );
	set_zd_data( "0810", mbfebillopnreg_v.recipientacc );
	set_zd_data( "0961", mbfebillopnreg_v.recipientname );
	set_zd_data( "0590", mbfebillopnreg_v.cashbillbkno );

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
