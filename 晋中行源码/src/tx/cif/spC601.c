/*************************************************
* 文 件 名: spC601.c
* 功能描述: 客户建议与投诉信息录入
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

spC601()
{
	struct	cif_req_log_c	sCifReqLog;		/* 客户反馈日志 */
	int 	ret, num;

	memset( &sCifReqLog, 0x00, sizeof( struct cif_req_log_c ) );

    /* 数据初始化 */
    pub_base_sysinit();

    /* 从前台屏幕取值 */
    get_zd_long( "0280", &sCifReqLog.cif_no );				/* 客户号 */
 	get_zd_data( "0810", sCifReqLog.request );             	/* 建议内容 */
   	get_zd_data( "0670", sCifReqLog.advice_type );          /* 建议类型 */
   	get_zd_data( "0920", sCifReqLog.req_clerk_no );         /* 投诉对象 */

	/* 检查客户反馈日志中是否已存在此记录 */
   	if( strcmp( sCifReqLog.req_clerk_no, "" ) == 0 )
    {
       	ret = Cif_req_log_Sel( g_pub_tx.reply, &sCifReqLog, \
				"cif_no=%ld and request='%s' and advice_type='%s' and \
				req_clerk_no is NULL", sCifReqLog.cif_no, \
				sCifReqLog.request, sCifReqLog.advice_type );
       	if( ret == 0 )
       	{
			sprintf( acErrMsg, "客户反馈日志中已存在此记录!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "C065" );
			goto ErrExit;
		}
		else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "读取客户反馈日志异常![%d]", ret );
       		WRITEMSG
       		goto ErrExit;
       	}
	}
	else
	{
		ret = Cif_req_log_Sel( g_pub_tx.reply, &sCifReqLog, \
				"cif_no=%ld and request='%s' and advice_type='%s' and \
				req_clerk_no='%s'", sCifReqLog.cif_no,\
				sCifReqLog.request, sCifReqLog.advice_type, \
				sCifReqLog.req_clerk_no );
       	if( ret == 0 )
       	{
			sprintf( acErrMsg, "客户反馈日志中已存在此记录!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "C065" );
			goto ErrExit;
		}
		else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "读取客户反馈日志异常![%d]", ret );
       		WRITEMSG
       		goto ErrExit;
       	}
	}
	
	/* 查询客户反馈日志中此客户号对应的最大序号 */
    num=sql_max_int("cif_req_log","seq_no","cif_no=%ld",sCifReqLog.cif_no);
	if(num<0)
  	{
      	sprintf( acErrMsg, "执行sql_max_int错误!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
    }

	/* 登记客户反馈日志 */
    sCifReqLog.seq_no = ++num;						/* 序号 */
    strcpy( sCifReqLog.clerk_no, g_pub_tx.tel );	/* 职员编号 */
    sCifReqLog.date = g_pub_tx.tx_date;				/* 建议时间 */

    ret =Cif_req_log_Ins( sCifReqLog, g_pub_tx.reply );
    if( ret == -239 )
    {
       	sprintf( acErrMsg, "登记客户反馈日志失败!有重复记录!" );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C063" );
       	goto ErrExit;
    }
	else if ( ret )
    {
       	sprintf( acErrMsg, "执行Cif_req_log_Ins错误!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
    }

    /* 记流水日志 */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_int("0480",num);

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
