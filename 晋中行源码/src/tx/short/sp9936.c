/*************************************************************
* 文 件 名:  sp9936.c
* 功能描述： 大额支付快捷查询
*			 大额挂帐提入交易
*			 根据来帐流水号和交易日期查询来帐交易信息
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

int sp9936()
{
	struct mbfeghvrecvlist_c ghvrecvlist_v;
	struct mbfe_cmt_fmt_c mbfe_cmt_fmt_v;
	struct mbfenetbank_c mbfenetbank_v;
	struct mbfeeisbank_c mbfeeisbank_v;

	memset(&ghvrecvlist_v, 0x00, sizeof(struct mbfeghvrecvlist_c));
	memset(&mbfe_cmt_fmt_v, 0x00, sizeof(struct mbfe_cmt_fmt_c));
	memset(&mbfenetbank_v, 0x00, sizeof(struct mbfenetbank_c));
	memset(&mbfeeisbank_v, 0x00, sizeof(struct mbfeeisbank_c));

	pub_base_sysinit();

	get_zd_long("0450",&ghvrecvlist_v.consigndate);
	get_zd_long("0530",&ghvrecvlist_v.flowno);

	g_reply_int=Mbfeghvrecvlist_Sel( g_pub_tx.reply, &ghvrecvlist_v,
		"rectime[1,8]='%ld' and flowno='%ld'", ghvrecvlist_v.consigndate, ghvrecvlist_v.flowno);
	if(g_reply_int==100)
	{
		sprintf( acErrMsg, "没有该笔来帐!" );
		WRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询来帐明细表数据库操作出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if (get_bkno()) goto ErrExit;
	if( strcmp( ghvrecvlist_v.cnapsrdfi, mbfetran.cnapsodfi ) )
	{
		sprintf( acErrMsg, "非本行接收来帐!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( ghvrecvlist_v.procstate[0] == '5' )
	{
		if( ghvrecvlist_v.procstate[1] == 'C' )
		{
			sprintf( acErrMsg, "汇票逾期退回资金请通过[4147]逾期兑付交易提入!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	else if( ghvrecvlist_v.procstate[0]=='0' )
	{
		sprintf( acErrMsg, "该笔来帐已清算，不能手工提入!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( ghvrecvlist_v.procstate[0]=='6' )
	{
		sprintf( acErrMsg, "该笔来帐已退回，不能手工提入!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( ghvrecvlist_v.procstate[0]=='7' )
	{
		sprintf( acErrMsg, "该笔来帐已手工提入，不能再次提入!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else
	{
		sprintf( acErrMsg, "该笔来帐状态不正常，不能手工提入!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfe_cmt_fmt_Sel( g_pub_tx.reply, &mbfe_cmt_fmt_v,
		"cmtno='%s'", ghvrecvlist_v.cmtno);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "查询报文格式定义表数据库操作出错![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(mbfe_cmt_fmt_v.cmt_name);

	set_zd_data("0230", ghvrecvlist_v.cmtno);
	set_zd_double("0390", ghvrecvlist_v.amount);
	set_zd_data("0580", ghvrecvlist_v.odficode);
	set_zd_data("0590", ghvrecvlist_v.rdficode);
	set_zd_data("0810", ghvrecvlist_v.payeracc);
	set_zd_data("0820", ghvrecvlist_v.recipientacc);
	set_zd_data("0860", ghvrecvlist_v.txssno);
	set_zd_data("0963", ghvrecvlist_v.payername);
	set_zd_data("0964", ghvrecvlist_v.recipientname);
	set_zd_data("0210", ghvrecvlist_v.procstate);
	pub_base_strpack( ghvrecvlist_v.odficode );
	pub_base_strpack( ghvrecvlist_v.rdficode );
	if( strlen( ghvrecvlist_v.odficode ) == 12 )
	{
		g_reply_int=Mbfenetbank_Sel( g_pub_tx.reply, &mbfenetbank_v,
			"nbkcode='%s'", ghvrecvlist_v.odficode);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "查询参与者行号表数据库操作出错![%d]", g_reply_int );
			MBFEWRITEMSG
		}
		set_zd_data("0961", mbfenetbank_v.nbkname);

		memset(&mbfenetbank_v, 0x00, sizeof(struct mbfenetbank_c));
		g_reply_int=Mbfenetbank_Sel( g_pub_tx.reply, &mbfenetbank_v,
			"nbkcode='%s'", ghvrecvlist_v.rdficode);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "查询参与者行号表数据库操作出错![%d]", g_reply_int );
			MBFEWRITEMSG
		}
		set_zd_data("0962", mbfenetbank_v.nbkname);
	}
	else if( strlen( ghvrecvlist_v.odficode ) == 6 )
	{
		g_reply_int=Mbfeeisbank_Sel( g_pub_tx.reply, &mbfeeisbank_v,
			"bnkno='%s'", ghvrecvlist_v.odficode);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "查询参与者行号表数据库操作出错![%d]", g_reply_int );
			MBFEWRITEMSG
		}
		set_zd_data("0961", mbfeeisbank_v.bnklname);

		memset(&mbfeeisbank_v, 0x00, sizeof(struct mbfeeisbank_c));
		g_reply_int=Mbfeeisbank_Sel( g_pub_tx.reply, &mbfeeisbank_v,
			"bnkno='%s'", ghvrecvlist_v.rdficode);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "查询参与者行号表数据库操作出错![%d]", g_reply_int );
			MBFEWRITEMSG
		}
		set_zd_data("0962", mbfeeisbank_v.bnklname);
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
