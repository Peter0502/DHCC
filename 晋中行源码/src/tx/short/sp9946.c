/*************************************************************
* 文 件 名:  sp9946.c
* 功能描述： 大额支付快捷查询
*			 大额挂帐提入子交易
*			 根据来帐流水号和交易日期检查来帐交易信息
*
* 作    者:  jane
* 完成日期： 2003年6月27日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9946()
{
	struct mbfeghvrecvlist_c mbfeghvrecvlist_v;

	memset(&mbfeghvrecvlist_v, 0x00, sizeof(struct mbfeghvrecvlist_c));

	pub_base_sysinit();

	get_zd_long("1234",&mbfeghvrecvlist_v.consigndate);
	get_zd_long("1235",&mbfeghvrecvlist_v.flowno);

	g_reply_int=Mbfeghvrecvlist_Sel( g_pub_tx.reply, &mbfeghvrecvlist_v,
		"consigndate='%ld' and flowno='%ld'", mbfeghvrecvlist_v.consigndate, mbfeghvrecvlist_v.flowno);
	if(g_reply_int==100)
	{
		vtcp_log( "[%ld][%ld]", mbfeghvrecvlist_v.consigndate,mbfeghvrecvlist_v.flowno);
		sprintf( acErrMsg, "没有该笔来帐!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询来帐明细表表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( mbfeghvrecvlist_v.procstate[0] == '5' )
	{
		if( mbfeghvrecvlist_v.procstate[1] == 'C' )
		{
			sprintf( acErrMsg, "汇票逾期退回资金请通过[4147]逾期兑付交易提入!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfeghvrecvlist_v.procstate[1] == 'D' )
		{
			sprintf( acErrMsg, "挂失汇票退回资金请通过[4147]逾期兑付交易提入!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	else if( mbfeghvrecvlist_v.procstate[0]=='0' )
	{
		sprintf( acErrMsg, "该笔来帐已清算，不能提入!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( mbfeghvrecvlist_v.procstate[0]=='6' )
	{
		sprintf( acErrMsg, "该笔来帐已退回，不能提入!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( mbfeghvrecvlist_v.procstate[0]=='7' )
	{
		sprintf( acErrMsg, "该笔来帐已手工提入，不能再次提入!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else
	{
		sprintf( acErrMsg, "该笔来帐状态不正常，不能提入!" );
		MBFEWRITEMSG
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
