/***************************************************************
* 文 件 名: spJ407.c
* 功能描述：质押融资通知来报处理
*
* 作    者: jane
* 完成日期：2004年07月15日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

spJ407()
{
	if (register_inpareg()) goto ErrExit;

OkExit:
	strcpy(g_pub_tx.reply,"0000");
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

int register_inpareg()
{
	struct mbfeinpawnmsg_c mbfeinpawnmsg_v;

	memset(&mbfeinpawnmsg_v,0x00,sizeof(struct mbfeinpawnmsg_c));

	mbfeinpawnmsg_v.consigndate=atol(mbfetran.consigndate);
	strcpy(mbfeinpawnmsg_v.spjoincode,mbfetran.odficode);
	strcpy(mbfeinpawnmsg_v.txssno,mbfetran.txssno);
	mbfeinpawnmsg_v.amount=atof(mbfetran.amount)/100;
	strcpy(mbfeinpawnmsg_v.osdficodeccpc,mbfetran.osdficodeccpc);
	strcpy(mbfeinpawnmsg_v.osdficode,mbfetran.osdficode);
	strcpy(mbfeinpawnmsg_v.isdficodeccpc,mbfetran.isdficodeccpc);
	strcpy(mbfeinpawnmsg_v.isdficode,mbfetran.isdficode);
	strcpy(mbfeinpawnmsg_v.procstate,"00");
	strcpy(mbfeinpawnmsg_v.checkstate,"00");
	sprintf(mbfetran.statetime,"%ld%06ld",g_pub_tx.tx_date,g_pub_tx.tx_time);
	strcpy(mbfeinpawnmsg_v.statetime,mbfetran.statetime);
	mbfeinpawnmsg_v.printno=0;
	mbfeinpawnmsg_v.flowno=g_pub_tx.trace_no;
	strcpy(mbfeinpawnmsg_v.errcode,mbfetran.errcode);
	strcpy(mbfeinpawnmsg_v.errdesc,mbfetran.errdesc);
	strcpy(mbfeinpawnmsg_v.strinfo,mbfetran.strinfo);

	sprintf(acErrMsg,"mbfeinpawnmsg: 1n[%ld] 2n[%s] 3n[%s] 4n[%.2f] 5n[%s] 6n[%s] 7n[%s] 8n[%s] 9n[%s] 10[%s] 11n[%s] 12[%d] 13n[%ld] 14[%s] 15[%s] 16[%s]",mbfeinpawnmsg_v.consigndate,mbfeinpawnmsg_v.spjoincode,mbfeinpawnmsg_v.txssno,mbfeinpawnmsg_v.amount,mbfeinpawnmsg_v.osdficodeccpc,mbfeinpawnmsg_v.osdficode,mbfeinpawnmsg_v.isdficodeccpc,mbfeinpawnmsg_v.isdficode,mbfeinpawnmsg_v.procstate,mbfeinpawnmsg_v.checkstate,mbfeinpawnmsg_v.statetime,mbfeinpawnmsg_v.printno,mbfeinpawnmsg_v.flowno,mbfeinpawnmsg_v.errcode,mbfeinpawnmsg_v.errdesc,mbfeinpawnmsg_v.strinfo);
	WRITEMSG

	g_reply_int=Mbfeinpawnmsg_Ins(mbfeinpawnmsg_v,g_pub_tx.reply);
	if ( g_reply_int )
	{
		sprintf(acErrMsg,"登记质押融资通知表数据库操作失败[%d]",g_reply_int);
		WRITEMSG
		return -1;
	}

	return 0;
}
