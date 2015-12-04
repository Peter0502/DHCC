/***************************************************************
* 文 件 名: spJ232.c
* 功能描述：即时转账借贷通知来报处理
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
#include "hv_define.h"
#include "hv_pub.h"
#include "find_debug.h"

spJ232()
{
	if (register_dcltreg()) goto ErrExit;

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

int register_dcltreg()
{
	struct mbferltxdclt_c mbferltxdclt_v;

	memset(&mbferltxdclt_v,0x00,sizeof(struct mbferltxdclt_c));

	mbferltxdclt_v.consigndate=atol(mbfetran.consigndate);
	strcpy(mbferltxdclt_v.bankcode,mbfetran.ccpcbkno);
	mbferltxdclt_v.txsbatno=atol(mbfetran.txsbatno);
	strcpy(mbferltxdclt_v.oprttype,mbfetran.oprttype2);
	strcpy(mbferltxdclt_v.txssno,mbfetran.txssno);
	mbferltxdclt_v.amount=atof(mbfetran.amount)/100;
	strcpy(mbferltxdclt_v.osdficodeccpc,mbfetran.osdficodeccpc);
	strcpy(mbferltxdclt_v.osdficode,mbfetran.osdficode);
	strcpy(mbferltxdclt_v.payeracc,mbfetran.payeracc);
	strcpy(mbferltxdclt_v.payername,mbfetran.payername);
	strcpy(mbferltxdclt_v.payerbnk,mbfetran.payeropenaccbkcode);
	strcpy(mbferltxdclt_v.isdficodeccpc,mbfetran.isdficodeccpc);
	strcpy(mbferltxdclt_v.isdficode,mbfetran.isdficode);
	strcpy(mbferltxdclt_v.recipientacc,mbfetran.recipientacc);
	strcpy(mbferltxdclt_v.recipientname,mbfetran.recipientname);
	strcpy(mbferltxdclt_v.recipientbnk,mbfetran.recipientopenaccbk);
	strcpy(mbferltxdclt_v.bondcode,mbfetran.bondcode);
	mbferltxdclt_v.bondamount=atof(mbfetran.bondamount)/100;
	mbferltxdclt_v.setamount=atof(mbfetran.setamount)/100;
	mbferltxdclt_v.setdate=atol(mbfetran.setdate);
	mbferltxdclt_v.setrate=atof(mbfetran.setrate)/100;
	strcpy(mbferltxdclt_v.bondordercode,mbfetran.bondordercode);
	mbferltxdclt_v.netamount=atof(mbfetran.netamount)/100;
	mbferltxdclt_v.bondrate=atof(mbfetran.bondrate)/100;
	strcpy(mbferltxdclt_v.procstate,"00");
	strcpy(mbferltxdclt_v.checkstate,"00");
	sprintf(mbfetran.statetime,"%ld%06ld",g_pub_tx.tx_date,g_pub_tx.tx_time);
	strcpy(mbferltxdclt_v.statetime,mbfetran.statetime);
	mbferltxdclt_v.printno=0;
	mbferltxdclt_v.flowno=g_pub_tx.trace_no;
	strcpy(mbferltxdclt_v.errcode,mbfetran.errcode);
	strcpy(mbferltxdclt_v.errdesc,mbfetran.errdesc);
	strcpy(mbferltxdclt_v.oprtsource,"01");
	strcpy(mbferltxdclt_v.strinfo,mbfetran.strinfo);

	sprintf(acErrMsg,"mbferltxdclt: 1n[%ld] 2n[%s] 3n[%ld] 4n[%s] 5n[%s] 6n[%.2f] 7n[%s] 8n[%s] 9[%s] 10[%s] 11[%s] 12n[%s] 13n[%s] 14[%s] 15[%s] 16[%s] 17[%s] 18n[%.2f] 19[%.2f] 20[%ld] 21[%.2f] 22[%s] 23[%.2f] 24[%.2f] 25n[%s] 26[%s] 27n[%s] 28[%d] 29n[%ld] 30[%s] 31[%s] 32n[%s] 33[%s]",mbferltxdclt_v.consigndate,mbferltxdclt_v.bankcode,mbferltxdclt_v.txsbatno,mbferltxdclt_v.oprttype,mbferltxdclt_v.txssno,mbferltxdclt_v.amount,mbferltxdclt_v.osdficodeccpc,mbferltxdclt_v.osdficode,mbferltxdclt_v.payeracc,mbferltxdclt_v.payername,mbferltxdclt_v.payerbnk,mbferltxdclt_v.isdficodeccpc,mbferltxdclt_v.isdficode,mbferltxdclt_v.recipientacc,mbferltxdclt_v.recipientname,mbferltxdclt_v.recipientbnk,mbferltxdclt_v.bondcode,mbferltxdclt_v.bondamount,mbferltxdclt_v.setamount,mbferltxdclt_v.setdate,mbferltxdclt_v.setrate,mbferltxdclt_v.bondordercode,mbferltxdclt_v.netamount,mbferltxdclt_v.bondrate,mbferltxdclt_v.procstate,mbferltxdclt_v.checkstate,mbferltxdclt_v.statetime,mbferltxdclt_v.printno,mbferltxdclt_v.flowno,mbferltxdclt_v.errcode,mbferltxdclt_v.errdesc,mbferltxdclt_v.oprtsource,mbferltxdclt_v.strinfo);
	WRITEMSG

	g_reply_int=Mbferltxdclt_Ins(mbferltxdclt_v,g_pub_tx.reply);
	if ( g_reply_int )
	{
		sprintf(acErrMsg,"登记即时转账借贷通知表数据库操作失败[%d]",g_reply_int);
		WRITEMSG
		return -1;
	}

	return 0;
}
