/*************************************************************
* 文 件 名:  sp9975.c
* 功能描述： 大额支付快捷查询
*			 大额退回往帐业务重发交易快查
*			 根据往帐支付序号和委托日期查询被退回的往帐交易信息
*			 并检查是否和退回业务退回的相符
*
* 作    者:  jane
* 完成日期： 2003年8月8日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9975()
{
	struct mbfeghvsendlist_c ghvsendlist_v;
	struct mbfeghvrecvlist_c ghvrecvlist_v;

	memset( &ghvsendlist_v, 0x00, sizeof( struct mbfeghvsendlist_c ) );
	memset( &ghvrecvlist_v, 0x00, sizeof( struct mbfeghvrecvlist_c ) );

	get_zd_long( "0440", &ghvsendlist_v.consigndate );    /*原往账委托日期*/
	get_zd_data( "0860", ghvsendlist_v.txssno );    /*原往账支付序号*/
	get_zd_long( "0450", &ghvrecvlist_v.consigndate );    /*退汇业务收到日期*/
	get_zd_long( "0530", &ghvrecvlist_v.flowno );    /*退汇业务流水号*/

	pub_base_sysinit();

	g_reply_int = Mbfeghvrecvlist_Sel( g_pub_tx.reply, &ghvrecvlist_v, 
		"rectime[1,8]='%ld' and flowno='%ld' and cmtno='108'", 
		ghvrecvlist_v.consigndate, ghvrecvlist_v.flowno );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "退汇业务不存在!" );
		WRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询来帐明细表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	/*退汇业务检查*/
	if( ghvrecvlist_v.procstate[0] == '5' )
	;
	else if( ghvrecvlist_v.procstate[0]=='6' )
	{
		sprintf( acErrMsg, "该笔退汇已退回，不能重发!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( ghvrecvlist_v.procstate[0]=='7' )
	{
		sprintf( acErrMsg, "该笔退汇已手工提入或重发，不能再次重发!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else
	{
		sprintf( acErrMsg, "该笔退汇来帐状态不正常，不能手工提入!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( ghvrecvlist_v.warrantdate != ghvsendlist_v.consigndate ||
		strcmp( ghvrecvlist_v.warrantno, ghvsendlist_v.txssno ) )
	{
		sprintf( acErrMsg, "该笔退汇业务与原往帐信息不符!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int = Mbfeghvsendlist_Sel( g_pub_tx.reply, &ghvsendlist_v, 
		"consigndate='%ld' and txssno='%s' and procstate='01'", 
		ghvsendlist_v.consigndate, ghvsendlist_v.txssno );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "退回的往帐业务不存在!" );
		WRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询往帐明细表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( ghvrecvlist_v.amount != ghvsendlist_v.amount )
	{
		sprintf( acErrMsg, "该笔退汇业务的金额与原往帐金额不符,请向退汇行查询!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( get_bkno() ) goto ErrExit;
	if( strcmp( ghvsendlist_v.cnapsodfi, mbfetran.cnapsodfi ) )
	{
		sprintf( acErrMsg, "非本行发起的往帐业务!" );
		MBFEWRITEMSG
		goto ErrExit;
	}


	set_zd_data( "0810", ghvsendlist_v.recipientacc );
	set_zd_data( "0961", ghvsendlist_v.recipientname );
	set_zd_data( "0300", ghvsendlist_v.payeracc );
	set_zd_data( "0250", ghvsendlist_v.payername );
	set_zd_double( "0390", ghvsendlist_v.amount );
	set_zd_data( "0580", ghvsendlist_v.rdficode );
	set_zd_data( "0964", ghvsendlist_v.strinfo );
	set_zd_data( "0240", ghvsendlist_v.cmtno );

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
