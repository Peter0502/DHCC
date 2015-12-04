/*****************************************************
* 文 件 名:  sp9767.c
* 功能描述： 根据退回申请号、申请日期、申请行号
			 查询原退回申请信息
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

int sp9767()
{
	struct mbfereturnapp_c	mbfereturnapp_v;

	memset(&mbfereturnapp_v,0x00,sizeof(struct mbfereturnapp_c));

	pub_base_sysinit();
	if (get_bkno()) goto ErrExit;

	get_zd_data("0240",mbfetran.origrtnappno);
	get_zd_long("0440",&g_pub_tx.erro_date);
	get_zd_data("0580",mbfetran.rdficode);

	if (!strcmp(mbfetran.rdficode,mbfetran.cnapsodfi))
	{
		sprintf(acErrMsg,"请输入它行行号");
		MBFEWRITEMSG
		goto ErrExit;
	}
	else 
	{
		g_reply_int=Mbfereturnapp_Sel(g_pub_tx.reply,&mbfereturnapp_v,
					"rtnappno='%s' and consigndate='%ld' and odficode='%s'",
					mbfetran.origrtnappno,g_pub_tx.erro_date,
					mbfetran.rdficode);
		if ( g_reply_int==100 )
		{
			sprintf(acErrMsg,"无符合该条件的退回申请，请核对后再提交！");
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if ( g_reply_int )
		{
			sprintf(acErrMsg,"查询大额退回申请登记簿出错[%d]",g_reply_int);
			MBFEWRITEMSG
			goto ErrExit;
		}

		if (!strcmp(mbfereturnapp_v.procstate,"00"))
		{
			sprintf(acErrMsg,"此退回申请书已经查复！");
			MBFEWRITEMSG
			goto ErrExit;
		}

		set_zd_data("0870",mbfereturnapp_v.origtxssno);
		set_zd_long("0450",mbfereturnapp_v.origconsigndate);
		set_zd_data("0590",mbfereturnapp_v.odficode);
		set_zd_double("0400",mbfereturnapp_v.origamount);
		set_zd_data("0961",mbfereturnapp_v.strinfo);
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
