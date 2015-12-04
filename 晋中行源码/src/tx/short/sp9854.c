/*************************************************
* 文 件 名:  sp9854.c
* 功能描述： 由客户号和序号回显建议内容和投诉对象（客户建议与投诉信息维护）
*
* 作    者:  jane
* 完成日期： 2003年2月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_req_log_c.h"

int sp9854()
{
	struct	cif_req_log_c	sCifReqLog;		/* 客户反馈日志 */
	int	ret;

	memset( &sCifReqLog, 0x00, sizeof( struct cif_req_log_c ) );
	
	get_zd_long( "0280", &g_pub_tx.cif_no );	/* 客户号 */
	get_zd_int( "0480", &g_pub_tx.ac_seqn );	/* 序号 */

	/* 检查客户反馈日志中是否存在 */
       	ret = Cif_req_log_Sel( g_pub_tx.reply, &sCifReqLog, "cif_no=%ld and seq_no=%d", \
        			g_pub_tx.cif_no, g_pub_tx.ac_seqn );
        if( ret == 100 )
        {
		sprintf( acErrMsg, "查询客户反馈日志错，无记录!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "C064" );
		goto ErrExit;
	}else if( ret )
        {
            	sprintf( acErrMsg, "读取客户反馈日志异常![%d]", ret );
            	WRITEMSG
            	goto ErrExit;
        }
	
	/* 向前台屏幕赋值 */
	set_zd_data( "0810", sCifReqLog.request );             	/* 建议内容 */
    	set_zd_data( "0920", sCifReqLog.req_clerk_no );         /* 投诉对象 */

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



