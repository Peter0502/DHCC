/*****************************************************
* 文 件 名:  sp9765.c
* 功能描述： 根据查询书号、查询日期、查询行号
			 查询原查询信息
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

int sp9765()
{
	char origqrycntt[5][61];
	char *tmp;
	struct mbfequerymsg_c	mbfequerymsg_v;

	memset(origqrycntt,0x00,sizeof(origqrycntt));
	memset(&mbfequerymsg_v,0x00,sizeof(struct mbfequerymsg_c));

	pub_base_sysinit();
	if (get_bkno()) goto ErrExit;

	get_zd_data("0860",mbfetran.origquerymsgno);
	get_zd_long("0440",&g_pub_tx.erro_date);
	get_zd_data("0580",mbfetran.origquerybkcode);

	if (!strcmp(mbfetran.origquerybkcode,mbfetran.cnapsodfi))
	{
		sprintf(acErrMsg,"请输入它行行号");
		MBFEWRITEMSG
		goto ErrExit;
	}
	else 
	{
		g_reply_int=Mbfequerymsg_Sel(g_pub_tx.reply,&mbfequerymsg_v,
					"querymsgno='%s' and querydate='%ld' and querybkcode='%s'",
					mbfetran.origquerymsgno,g_pub_tx.erro_date,
					mbfetran.origquerybkcode);
		if ( g_reply_int==100 )
		{
			sprintf(acErrMsg,"无符合该条件的查询书，请核对后再提交！");
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if ( g_reply_int )
		{
			sprintf(acErrMsg,"查询大额查询书登记簿出错[%d]",g_reply_int);
			MBFEWRITEMSG
			goto ErrExit;
		}

		if (!strcmp(mbfequerymsg_v.procstate,"00"))
		{
			sprintf(acErrMsg,"此查询书已经查复！");
			MBFEWRITEMSG
			goto ErrExit;
		}

		tmp=mbfequerymsg_v.querycontent;
		tmp=pub_base_strn_cpyc(tmp,1,60,origqrycntt[0]);
		tmp=pub_base_strn_cpyc(tmp,1,60,origqrycntt[1]);
		tmp=pub_base_strn_cpyc(tmp,1,60,origqrycntt[2]);
		tmp=pub_base_strn_cpyc(tmp,1,60,origqrycntt[3]);

		set_zd_data("0870",mbfequerymsg_v.origtxssno);
		set_zd_long("0450",mbfequerymsg_v.origconsigndate);
		set_zd_data("0590",mbfequerymsg_v.origodficode);
		set_zd_double("0400",mbfequerymsg_v.origamount);
		set_zd_data("0961",origqrycntt[0]);
		set_zd_data("0962",origqrycntt[1]);
		set_zd_data("0963",origqrycntt[2]);
		set_zd_data("0964",origqrycntt[3]);
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
