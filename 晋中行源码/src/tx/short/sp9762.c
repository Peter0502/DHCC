/*****************************************************
* 文 件 名:  sp9762.c
* 功能描述： 根据支付交易序号、发报日期、发起行号
			 查询原业务信息
* 作    者:  jane
* 完成日期： 2003年6月3日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "mbfe_pub.h"

int sp9762()
{
	struct mbfeghvsendlist_c	mbfeghvsendlist_v;
	struct mbfeghvrecvlist_c	mbfeghvrecvlist_v;
	struct mbfenetbank_c	mbfenetbank_v;

	memset(&mbfeghvsendlist_v,0x00,sizeof(struct mbfeghvsendlist_c));
	memset(&mbfeghvrecvlist_v,0x00,sizeof(struct mbfeghvrecvlist_c));
	memset(&mbfenetbank_v,0x00,sizeof(struct mbfenetbank_c));

	pub_base_sysinit();
	if (get_bkno()) goto ErrExit;

	get_zd_data("0860",mbfetran.origtxssno);
	get_zd_long("0440",&g_pub_tx.erro_date);
	get_zd_data("0590",mbfetran.origodficode);

	if (!strcmp(mbfetran.origodficode,mbfetran.cnapsodfi))
	{
		g_reply_int=Mbfeghvsendlist_Sel(g_pub_tx.reply,&mbfeghvsendlist_v,
					"txssno='%s' and consigndate='%ld' and odficode='%s'",
					mbfetran.origtxssno,g_pub_tx.erro_date,
					mbfetran.origodficode);
		if ( g_reply_int==100 )
		{
			sprintf(acErrMsg,"查询的往帐交易不存在，请核对后再提交！");
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if ( g_reply_int )
		{
			sprintf(acErrMsg,"查询大额往帐明细表出错[%d]",g_reply_int);
			MBFEWRITEMSG
			goto ErrExit;
		}

		g_reply_int=Mbfenetbank_Sel(g_pub_tx.reply,&mbfenetbank_v,
					"nbkcode='%s'",mbfeghvsendlist_v.rdficode);
		if ( g_reply_int )
		{
			sprintf(acErrMsg,"查询查复行名称出错[%d]",g_reply_int);
			MBFEWRITEMSG
			goto ErrExit;
		}

		set_zd_data("0580",mbfeghvsendlist_v.rdficode);
		set_zd_data("0830",mbfenetbank_v.nbkname);
		set_zd_data("0963",mbfeghvsendlist_v.payeracc);
		set_zd_data("0961",mbfeghvsendlist_v.payername);
		set_zd_data("0964",mbfeghvsendlist_v.recipientacc);
		set_zd_data("0962",mbfeghvsendlist_v.recipientname);
		set_zd_double("0400",mbfeghvsendlist_v.amount);
	}
	else 
	{
		g_reply_int=Mbfeghvrecvlist_Sel(g_pub_tx.reply,&mbfeghvrecvlist_v,
					"txssno='%s' and consigndate='%ld' and odficode='%s'",
					mbfetran.origtxssno,g_pub_tx.erro_date,
					mbfetran.origodficode);
		if ( g_reply_int==100 )
		{
			sprintf(acErrMsg,"查询的来帐交易不存在，请核对后再提交！");
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if ( g_reply_int )
		{
			sprintf(acErrMsg,"查询大额来帐明细表出错[%d]",g_reply_int);
			MBFEWRITEMSG
			goto ErrExit;
		}

		g_reply_int=Mbfenetbank_Sel(g_pub_tx.reply,&mbfenetbank_v,
					"nbkcode='%s'",mbfeghvrecvlist_v.odficode);
		if ( g_reply_int )
		{
			sprintf(acErrMsg,"查询查复行名称出错[%d]",g_reply_int);
			MBFEWRITEMSG
			goto ErrExit;
		}

		set_zd_data("0580",mbfeghvrecvlist_v.odficode);
		set_zd_data("0830",mbfenetbank_v.nbkname);
		set_zd_data("0963",mbfeghvrecvlist_v.payeracc);
		set_zd_data("0961",mbfeghvrecvlist_v.payername);
		set_zd_data("0964",mbfeghvrecvlist_v.recipientacc);
		set_zd_data("0962",mbfeghvrecvlist_v.recipientname);
		set_zd_double("0400",mbfeghvrecvlist_v.amount);
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
