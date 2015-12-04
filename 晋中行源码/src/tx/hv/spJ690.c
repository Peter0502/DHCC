/*************************************************
* 文 件 名:	spJ690.c
* 功能描述：业务明细下载报文子交易
* 作    者: jane
* 完成日期: 2004年6月30日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/

#define	EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "com_parm_c.h"
#include "find_debug.h"

spJ690()
{
	struct com_parm_c com_parm_v;

	if (g_pub_tx.tmp_amt2>0)
	{
		memset(&com_parm_v,0x00,sizeof(struct com_parm_c));
		g_reply_int=Com_parm_Sel(g_pub_tx.reply, &com_parm_v, "parm_code='DEKM' and parm_seqn='1'");
		if (g_reply_int)
		{
			sprintf(acErrMsg,"查询大额往账账号出错 [%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1201",com_parm_v.val);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tmp_amt2);
		set_zd_data("120A","大额清算补记");

		memset(&com_parm_v,0x00,sizeof(struct com_parm_c));
		g_reply_int=Com_parm_Sel(g_pub_tx.reply, &com_parm_v, "parm_code='DEKM' and parm_seqn='3'");
		if (g_reply_int)
		{
			sprintf(acErrMsg,"查询日间清算账号出错 [%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1211",com_parm_v.val);
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tmp_amt2);
		set_zd_data("121A","大额清算补记");

		strcpy(g_pub_tx.reply,"0002");
		goto OkExit1;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
