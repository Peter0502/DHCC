/*************************************************************
* 文 件 名:  sp9789.c
* 功能描述： 大额支付快捷查询
*			 退汇支付根据来帐流水号和交易日期查询来帐交易信息
*
* 作    者:  jane
* 完成日期： 2003年6月8日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9789()
{
	char date[9];
	struct mbfeghvrecvlist_c mbfeghvrecvlist_v;
	struct mbfe_cmt_fmt_c mbfe_cmt_fmt_v;
	struct mbfenetbank_c mbfenetbank_v;

	memset(&mbfeghvrecvlist_v, 0x00, sizeof(struct mbfeghvrecvlist_c));
	memset(&mbfe_cmt_fmt_v, 0x00, sizeof(struct mbfe_cmt_fmt_c));
	memset(&mbfenetbank_v, 0x00, sizeof(struct mbfenetbank_c));

	pub_base_sysinit();

	get_zd_long("0450",&mbfeghvrecvlist_v.consigndate);
	get_zd_long("0530",&mbfeghvrecvlist_v.flowno);
	sprintf(date,"%8ld",mbfeghvrecvlist_v.consigndate);

	g_reply_int=Mbfeghvrecvlist_Sel( g_pub_tx.reply, &mbfeghvrecvlist_v,
		"rectime[1,8]='%s' and flowno='%ld'", date, mbfeghvrecvlist_v.flowno);
	if(g_reply_int==100)
	{
		sprintf( acErrMsg, "没有该笔来帐!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询来帐明细表表数据库操作出错[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( mbfeghvrecvlist_v.procstate[0]!='5' )
	{
		if( mbfeghvrecvlist_v.procstate[0]=='0' )
		{
			sprintf( acErrMsg, "该笔来帐已清算，不能退汇!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfeghvrecvlist_v.procstate[0]=='6' )
		{
			sprintf( acErrMsg, "该笔来帐已退回，不能退汇!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfeghvrecvlist_v.procstate[0]=='7' )
		{
			sprintf( acErrMsg, "该笔来帐已手工提入，不能退汇!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else
		{
			sprintf( acErrMsg, "该笔来帐状态不正常，不能退汇!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}

	if( get_bkno() ) goto ErrExit;
	if( strcmp(mbfeghvrecvlist_v.cnapsrdfi, mbfetran.cnapsodfi) )
	{
		sprintf( acErrMsg, "非本行接收的来帐业务，不能退汇!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfe_cmt_fmt_Sel( g_pub_tx.reply, &mbfe_cmt_fmt_v,
		"cmtno='%s'", mbfeghvrecvlist_v.cmtno);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "查询报文格式定义表数据库操作出错![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(mbfe_cmt_fmt_v.cmt_name);

	if( strcmp(mbfeghvrecvlist_v.cmtno,"100") &&
		strcmp(mbfeghvrecvlist_v.cmtno,"101") &&
		strcmp(mbfeghvrecvlist_v.cmtno,"102") )
	{
		sprintf( acErrMsg, "%s来帐业务不能退汇!", mbfe_cmt_fmt_v.cmt_name );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfenetbank_Sel( g_pub_tx.reply, &mbfenetbank_v,
		"nbkcode='%s'", mbfeghvrecvlist_v.odficode);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "查询参与者行号表数据库操作出错![%d]", g_reply_int );
		MBFEWRITEMSG
	}

	set_zd_data("0860", mbfeghvrecvlist_v.txssno);
	set_zd_data("0230", mbfeghvrecvlist_v.procstate);
	set_zd_data("0240", mbfeghvrecvlist_v.cmtno);
	set_zd_data("0870", mbfe_cmt_fmt_v.cmt_name);
	set_zd_data("0580", mbfeghvrecvlist_v.odficode);
	set_zd_data("0963", mbfenetbank_v.nbkname);
	set_zd_double("0390", mbfeghvrecvlist_v.amount);
	set_zd_data("0300", mbfeghvrecvlist_v.recipientacc);
	mbfeghvrecvlist_v.recipientname[50]='\0';
	set_zd_data("0250", mbfeghvrecvlist_v.recipientname);
	set_zd_data("0810", mbfeghvrecvlist_v.payeracc);
	set_zd_data("0961", mbfeghvrecvlist_v.payername);

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
