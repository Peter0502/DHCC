/*************************************************
* 文 件 名: spC602.c
* 功能描述: 客户建议与投诉信息维护
* 作    者: andy
* 完成日期：2004年2月19日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_req_log_c.h"

spC602()
{
	struct	cif_req_log_c	sCifReqLog;		/* 客户反馈日志 */
	char	cOptcode[2];					/* 操作码 */
	char	cRequest[41];					/* 建议内容 */
	char	cReqClerkNo[5];					/* 投诉对象 */
	char	comm[100];						/* 删除表记录的where条件 */
	int 	ret;
 	
	memset( &sCifReqLog, 0x00, sizeof( struct cif_req_log_c ) );
	memset( cOptcode, 0x00, sizeof( cOptcode ) );
	memset( cRequest, 0x00, sizeof( cRequest ) );
	memset( cReqClerkNo, 0x00, sizeof( cReqClerkNo ) );
	memset( comm, 0x00, sizeof( comm ) );

        /* 数据初始化 */
        pub_base_sysinit();

        /* 从前台屏幕取值 */
        get_zd_data( "0670", cOptcode );  			/* 操作码: 1修改 2删除 */
        get_zd_long( "0280", &sCifReqLog.cif_no );		/* 客户号 */
        get_zd_long( "0480", &sCifReqLog.seq_no );		/* 序号 */
    	get_zd_data( "0810", cRequest );  			/* 建议内容 */
    	get_zd_data( "0920", cReqClerkNo );             	/* 投诉对象 */
       	
       	/* 如果是修改操作 */
       	if( cOptcode[0] == '1' )
       	{
       		/* 检查客户反馈日志 */
		ret = Cif_req_log_Dec_Upd( g_pub_tx.reply , "cif_no=%ld and seq_no=%d", \
						sCifReqLog.cif_no, sCifReqLog.seq_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Cif_req_log_Dec_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 取出记录 */
		ret = Cif_req_log_Fet_Upd( &sCifReqLog, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "查询客户反馈日志错，无记录!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "C064" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Cif_req_log_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 修改客户反馈日志 */
		strcpy( sCifReqLog.request, cRequest );
		strcpy( sCifReqLog.req_clerk_no, cReqClerkNo );
		
		ret = Cif_req_log_Upd_Upd( sCifReqLog, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改客户反馈日志错[%d]", ret );
			WRITEMSG
			strcpy( g_pub_tx.reply, "C074" );
			goto ErrExit;	
		}
		
		Cif_req_log_Clo_Upd();
	}
	else		/* 如果是删除操作 */
        {
        	/* 准备删除表记录的where条件 */
        	sprintf( comm, "cif_no=%ld and seq_no=%d and request='%s' \
        		and req_clerk_no='%s'",sCifReqLog.cif_no, sCifReqLog.seq_no, \
        		cRequest, cReqClerkNo );

        	ret = deldb( "cif_req_log", comm );
        	if( ret )
        	{
        		sprintf( acErrMsg, "删除表记录错[%d]", ret );
			WRITEMSG
			goto ErrExit;
        	}
	}
	
        /* 记流水日志 */
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
