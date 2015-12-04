/*************************************************
* 文 件 名:  spJ910.c
* 功能描述： 910通用回应报文处理子交易
*			 拆报文
*			 对301,302,303,313,314,721,724类型报文
*            的910回应分别修改对应的登记簿的错误代
*            码字段,对其他类型报文的910回应不做处
*            理
*			 
* 作    者:  jane
* 完成日期： 2004年7月19日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

int spJ910()
{
	struct mbfesendcmtmsg_c sendcmtmsg_v;

	g_reply_int = Mbfesendcmtmsg_Sel( g_pub_tx.reply, &sendcmtmsg_v, "respno='%s'", mbfetran.respno );
	if (g_reply_int)
	{
		sprintf( acErrMsg,"查询报文参考号为[%s]的往包信息出错,[%ld]", mbfetran.respno, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	switch( atoi(sendcmtmsg_v.cmtno ) )
	{
		case 301:
			g_reply_int=sql_execute("update mbfequerymsg set errcode='%s', errdesc='%s' where querymsgno='%s' and querydate='%ld' and querysource='00'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910回执修改查询书登记簿出错[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 302:
			g_reply_int=sql_execute("update mbfereplymsg set errcode='%s', errdesc='%s' where rplymsgno='%s' and rplydate='%ld' and rplysource='00'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910回执修改查复书登记簿出错[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 303:
			g_reply_int=sql_execute("update mbfefreeinfo set errcode='%s', errdesc='%s' where msssno='%s' and consigndate='%ld' and infosource='00'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910回执修改自由格式报文登记簿出错[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 313:
			g_reply_int=sql_execute("update mbfereturnapp set errcode='%s', errdesc='%s' where rtnappno='%s' and consigndate='%ld' and rtnappsource='00'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910回执修改退回申请登记簿出错[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 314:
			g_reply_int=sql_execute("update mbfereturnrply set errcode='%s', errdesc='%s' where origrtnappno='%s' and origrtnappdate='%ld' and rdficode='%s'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.origrtnappdate,sendcmtmsg_v.bkcode);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910回执修改退回应答登记簿出错[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 651:
			g_reply_int=sql_execute("insert into mbfeodoprtqryrpl values ('%ld','%s','','','','','','','','','','','','','','','%s','%s')",
				sendcmtmsg_v.cnapsdate,sendcmtmsg_v.respno,mbfetran.errcode,mbfetran.strinfo);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910回执登记往帐状态查询回复登记簿出错[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 721:
			g_reply_int=sql_execute("update mbfebillaplset set errcode='%s', errdesc='%s' where msssno='%s' and consigndate='%ld'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910回执修改银行汇票兑付申请登记簿出错[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 724:
			g_reply_int=sql_execute("update mbfebillrtureg set errcode='%s', errdesc='%s' where msssno='%s' and consigndate='%ld'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910回执修改银行汇票退回申请登记簿出错[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;

		default:break;
	}

OkExit:
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
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
