#include "find_debug.h" 
/*************************************************
* 文 件 名:     pub_base_trace
* 功能描述：    1、包括模块pub_base_PubQlsh取得当前可用的主机流水号
                2、包括模块pub_base_PubQlsh_Trans取得当前可用的主机流水号
                3、包括模块pub_base_getwdlsh取得当前可用的网点流水号
* 作    者:
* 完成日期：    2003年12月16日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"

/**********************************************************************
 * 函 数 名：   pub_base_PubQlsh
 * 函数功能：   取得当前可用的主机流水号
 * 作者/时间：
 *
 * 参数：
 *     输入: 无
 *     输出: 无
 *     返回: <=0失败
             >0取得的主机流水号
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_init_trace_no()
{
 	long tlsrno;
	long max_trace_no;
	int ret=0;

		TRACE;

  	ret=sql_max_long ( "trace_log","trace_no",&max_trace_no,"1=1" );
	if( ret )
	{
		TRACE;
		goto ErrExit;
	}
		TRACE;

 	tlsrno = get_tlsrno();
 	if ( tlsrno<0 )
 	{
 		sprintf(acErrMsg,"取主机流水号错 ! zjlsh=[%d]",tlsrno);
 		WRITEMSG
		/*goto ErrExit; */
 	}
 		sprintf(acErrMsg,"主机流水号 ![%d] tab[%d]",tlsrno,max_trace_no);
 		WRITEMSG
		TRACE;

	/*if( max_trace_no>=tlsrno )*/
	{
		set_tlsrno( max_trace_no+1 );
		printf( " SET TRACE NO to [%d]\n",max_trace_no+1 );
		sprintf( acErrMsg," SET TRACE NO to [%d]",max_trace_no+1 );
		WRITEMSG
	}

	TRACE;
	return 0;
ErrExit:
	strcpy ( g_pub_tx.reply,"52T1" );
	TRACE;
 	return -1;
}
int pub_base_PubQlsh(long *zjlsh,char *xym)
{
 long tlsrno;
 char tmp[21];

 MEMSET_DEBUG(tmp,0x00,sizeof(tmp));

 tlsrno = get_tlsrno();

 if ( tlsrno<0 )
 {
 sprintf(acErrMsg,"取主机流水号错 ! zjlsh=[%d]",tlsrno);
 WRITEMSG
 strcpy ( xym , "52T1" );
 return -1;
 }

        *zjlsh=tlsrno;

        return (0);
}

/**********************************************************************
 * 函 数 名：   pub_base_PubQlsh_Trans
 * 函数功能：   日终业务取得当前可用的主机流水号
 * 作者/时间：
 *
 * 参数：
 *     输入: 无
 *     输出: zhlsh
 *     返回: <=0失败
	     >0取得的主机流水号
 *
 * 修改历史：
 *
********************************************************************/

int pub_base_PubQlsh_Trans( long *zjlsh)
{
        long tlsrno;

        tlsrno = get_tlsrno();

        *zjlsh=tlsrno;

        return (0);
}
