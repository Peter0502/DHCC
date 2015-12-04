/*************************************************
* 文 件 名:  sp9871.c
* 功能描述： 由流水号回显流水日志信息
*
* 作    者:  jane
* 完成日期： 2003年3月25日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"

int sp9871()
{
	struct	trace_log_c	sTraceLog;	/* 业务流水表 */
	long	lNoteNum;			/* 凭证数量 */
	int	ret;

	memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
	
	get_zd_long( "0520", &sTraceLog.trace_no );	/* 流水号 */

	/* 检查业务流水表中是否存在此流水号 */
	ret = Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%ld", sTraceLog.trace_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "业务流水表中不存在此流水号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N067" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Trace_log_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	lNoteNum = (long)sTraceLog.amt;	/* 此处金额存放凭证数量 */	

	/* 向前台屏幕赋值 */
	set_zd_data( "0300", sTraceLog.ac_no );	/* 账号 */
	set_zd_data( "0230", sTraceLog.note_type );	/* 凭证种类 */
	set_zd_data( "0580", sTraceLog.note_no );	/* 凭证号 */
	set_zd_long( "0530", lNoteNum );		/* 凭证数量 */
	set_zd_data( "0920", sTraceLog.tel );		/* 操作员 */

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




