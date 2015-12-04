/*************************************************
* 文 件 名:  sp9981.c
* 功能描述： 根据流水，交易名称快查 
*
* 作    者:  jane
* 完成日期： 2003年8月16日 
*
* 修改记录： 
*   日   期: 20060824
*   修 改 人:ligl
*   修改内容:
*   日   期: 20100607
*   修 改 人:Han Xichao
*   修改内容: 增加查询时从trace_log表中返回交易代码,账号,发生额等信息
update tx_def set bit_map='10000000000000000000000000000000100101000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000' where tx_code='9981';
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "tx_def_c.h"

int sp9981()
{
	struct	trace_log_c	sTraceLog;	/* 业务流水 */
	struct	tx_def_c	sTx_def;		/* 交易定义结构 */
	int	ret;
	char   tx_code[5];

	memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
	memset( &sTx_def, 0x00, sizeof( struct tx_def_c ) );
	memset( tx_code, 0x00, sizeof(tx_code ) );

	get_zd_long( "0440", &g_pub_tx.trace_no );	/* 主机流水号 */
	get_zd_long( "0050", &g_pub_tx.tx_date);
	/* 根据主机流水号查询业务流水表 */
	ret = Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%ld and tx_date=%ld", g_pub_tx.trace_no,g_pub_tx.tx_date );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此流水号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S045" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Trace_log_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	vtcp_log( "[%s],[%d]  tx_code=[%s]", __FILE__,__LINE__,sTraceLog.tx_code );
	/**Add by HXC 100607 Begin**/
	memset( tx_code, 0x00, sizeof(tx_code) );
	memcpy(tx_code,sTraceLog.tx_code,4);
	vtcp_log( "[%s],[%d]  tx_code=[%s]", __FILE__,__LINE__,tx_code );
	if(memcmp(tx_code,"5802",4)==0)
		{
			memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
			ret = Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%ld and tx_date=%ld and tx_code='%s' and sub_tx_code='DEWZ'", g_pub_tx.trace_no,g_pub_tx.tx_date,tx_code );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此流水号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S045" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Trace_log_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	 }
	}
	if(memcmp(tx_code,"5842",4)==0 || memcmp(tx_code,"5844",4)==0 || memcmp(tx_code,"5846",4)==0)
		{
			memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
			ret = Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%ld and tx_date=%ld and tx_code='%s' and sub_tx_code='XEJZ'", g_pub_tx.trace_no,g_pub_tx.tx_date,tx_code );
	    if( ret == 100 )
	     {
		     sprintf( acErrMsg, "无此流水号!" );
		     WRITEMSG
		     strcpy( g_pub_tx.reply, "S045" );
		     goto ErrExit;
   	   }
   	  else if( ret )
   	  {
   		 sprintf( acErrMsg, "执行Trace_log_Sel错![%d]", ret );
		   WRITEMSG
		   goto ErrExit;
	    }
	  }
	vtcp_log( "[%s],[%d]  tx_code=[%s],ac_no=[%s],amt=[%.2f]", __FILE__,__LINE__,tx_code,sTraceLog.ac_no,sTraceLog.amt);
	set_zd_data( "0360", tx_code);			/* 交易码 */
	set_zd_data( "0330", sTraceLog.ac_no);			 /* 交易账号 */
	set_zd_double( "0420", sTraceLog.amt);			/* 交易发生额 */
	/**Add by HXC 100607 End**/
	/* 根据交易代码查询菜单结构 */
	ret = Tx_def_Sel( g_pub_tx.reply, &sTx_def, "tx_code='%s'", tx_code );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此交易代码!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S061" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Tx_def_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log( "[%s],[%d]  tx_name[%s]", __FILE__,__LINE__,sTx_def.tx_name );
	set_zd_data( "0380", sTx_def.tx_name);			/* 交易名称 */

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
