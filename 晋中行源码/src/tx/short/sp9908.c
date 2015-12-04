/*************************************************
* 文 件 名:    sp9908.c
* 功能描述：   
*              输入流水号显示账号、户名和余额
*
* 作    者:    
* 完成日期：   2003年05月13日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
	
sp9908()  
{ 		
	struct  trace_log_c		sTraceLog;
	struct  dd_mst_c		sDdMst;
	char	name[51];
	int ret=0;
	
	/** 数据初始化 **/
	memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	get_zd_long( "0280" , &g_pub_tx.trace_no );	/* 流水号 */

	ret= Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%d and \
	 tx_br_no='%s' and \
	 (sub_tx_code='D099' or sub_tx_code='D002' or sub_tx_code='A017') and  \
	 svc_ind<9000 " ,g_pub_tx.trace_no,g_pub_tx.tx_br_no);
	if( ret == 100 )
	{
		sprintf( acErrMsg, "流水号不存在,或该笔流水非存款交易![%ld]",g_pub_tx.trace_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P297" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "取交易流水异常出错![%d][%d]",ret,g_pub_tx.trace_no);
		strcpy( g_pub_tx.reply, "P298" );
		WRITEMSG
		goto ErrExit;
	}

	/* jane于2003.7.27封
	pub_base_strpack(sTraceLog.tel);
	if( strcmp(g_pub_tx.tel,sTraceLog.tel) )
	{
		sprintf( acErrMsg,"只有做该笔交易的柜员可以确认![%s][%s]",sTraceLog.tel,g_pub_tx.tel);
		strcpy( g_pub_tx.reply, "P299" );
		WRITEMSG
		goto ErrExit;
	}*/

/**
	if( sTraceLog.saving_notarize[0] == '1' )
	{
		sprintf( acErrMsg, "该笔储蓄存款业务已经确任过了!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P300" );
		goto ErrExit;
	}
**/
	if( sTraceLog.saving_notarize[0]=='1' )
		set_zd_data( "0810", "已经确认" );
	else
		set_zd_data( "0810", "未确认" );

	ret= Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%d and  ac_seqn=%d",sTraceLog.ac_id,sTraceLog.ac_seqn);
	if( ret )
	{
		sprintf( acErrMsg, "取活期主文件异常出错![%d][%d][%d]",ret,sTraceLog.ac_id,sTraceLog.ac_seqn);
		strcpy( g_pub_tx.reply, "W015" );
		WRITEMSG
		goto ErrExit;
	}

	/* 输出变量 */
	set_zd_data( "0300", sTraceLog.ac_no );	/* 帐/卡号 */	
	set_zd_long( "0340", sTraceLog.ac_seqn );/* 账号序号 */
	set_zd_double( "0400", sTraceLog.amt );	/* 存款金额 */
	set_zd_data( "0250", sDdMst.name );		/* 姓名 */
	/*set_zd_data("0920",sTraceLog.tel);*//*柜员*/
	
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
