/***************************************************************
* 文 件 名:     spD2A8.c
* 功能描述：	储蓄存款确认
* 作    者:     jack
* 完成日期：    2004年05月13日
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
#include "trace_log_c.h"

spD2A8()
{
	struct	trace_log_c	sTraceLog;
	memset(&sTraceLog,0x00,sizeof(struct trace_log_c));
	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	get_zd_long("0280",&g_pub_tx.trace_no);

	g_reply_int=Trace_log_Dec_Upd(g_pub_tx.reply,"trace_no=%d and \
		tx_br_no='%s' \
		and (sub_tx_code='D099' or sub_tx_code='D002' or sub_tx_code='A017') \
		and svc_ind<9000 ",g_pub_tx.trace_no,g_pub_tx.tx_br_no);
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取交易流水异常出错![%d][%d]",g_reply_int,g_pub_tx.trace_no);
		strcpy( g_pub_tx.reply, "P298" );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int=Trace_log_Fet_Upd(&sTraceLog,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf( acErrMsg, "流水号不存在,或该笔流水非存款交易![%d]",g_pub_tx.trace_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P297" );
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "取交易流水异常出错![%d][%d]",g_reply_int,g_pub_tx.trace_no);
		strcpy( g_pub_tx.reply, "P298" );
		WRITEMSG
		goto ErrExit;
	}

/*	zy于2005.7.27封
	pub_base_strpack(sTraceLog.tel);
	if( strcmp(g_pub_tx.tel,sTraceLog.tel) )
	{
		sprintf( acErrMsg,"只有做该笔交易的柜员可以确认![%s][%s]",sTraceLog.tel,g_pub_tx.tel);
		strcpy( g_pub_tx.reply, "P299" );
		WRITEMSG
		goto ErrExit;
	}
*/
	if( sTraceLog.saving_notarize[0] == '1' )
	{
		sprintf( acErrMsg, "该笔储蓄存款业务已经确任过了!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P300" );
		goto ErrExit;
	}

	strcpy(sTraceLog.saving_notarize,"1");
	set_zd_data("0810","已经确认");
	g_reply_int=Trace_log_Upd_Upd(sTraceLog,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取交易流水异常出错![%d][%d]",g_reply_int,g_pub_tx.trace_no);
		strcpy( g_pub_tx.reply, "P298" );
		WRITEMSG
		goto ErrExit;
	}

	Trace_log_Clo_Upd();

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"testw款成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"wstudttest款失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
