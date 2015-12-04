/*************************************************
* 文 件 名:  spJ253.c
* 功能描述： 大额往账清算结果返回
*			 修改往账登记簿未回执状态为相应的状态
*			 登记回执信息
*			 根据返回的状态码走不同出口
*			 
* 作    者:  janeg
* 完成日期： 2003年5月15日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "hv_pub.h"
#include "com_parm_c.h"
#include "find_debug.h"

int spJ253()
{
	char tmpstr[30];

	struct mbfeghvsendlist_c mbfeghvsendlist_v;
	struct com_parm_c com_parm_v;

	memset(&mbfeghvsendlist_v,0x00,sizeof(struct mbfeghvsendlist_c));
	memset(&com_parm_v,0x00,sizeof(struct com_parm_c));

    sprintf(acErrMsg,"\n往账业务回执:支付交易序号[%s]委托日期[%s]发起行[%s]",
		mbfetran.txssno,mbfetran.consigndate,mbfetran.odficode);
    WRITEMSG

	g_reply_int = Mbfeghvsendlist_Dec_Upd(g_pub_tx.reply, "txssno='%s' and consigndate='%s'",
		mbfetran.txssno,mbfetran.consigndate);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"查询业务回执对应的往账交易 声明游标数据库操作出错![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = Mbfeghvsendlist_Fet_Upd(&mbfeghvsendlist_v,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"查询业务回执对应的往账交易 取游标数据库操作出错![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	if (!strcmp(mbfetran.errcode,"CH2I0000") ||
		!strcmp(mbfetran.errcode,"CS2I0000"))
	{
		strcpy(mbfeghvsendlist_v.procstate, "00");	/*已清算*/
		strcpy(g_pub_tx.reply,"0001");
		set_zd_double("0390", mbfeghvsendlist_v.amount);
		g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, 
			"parm_code='DEKM' and parm_seqn='3'" );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"取大额清算账号数据库操作失败![%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data( "0300", com_parm_v.val );
	}
	else if (!strcmp(mbfetran.errcode,"CS2O9010"))
	{
		strcpy(mbfeghvsendlist_v.procstate, "20");	/*排队*/
		strcpy(g_pub_tx.reply,"0000");
	}
	else
	{
		strcpy(mbfeghvsendlist_v.procstate, "30");	/*滞留*/
		strcpy(g_pub_tx.reply,"0000");
	}

	sprintf(mbfeghvsendlist_v.settime, "%8ld%06ld", g_pub_tx.tx_date, g_pub_tx.tx_time);
	strcpy(mbfeghvsendlist_v.errcode, mbfetran.errcode);
	mbfeghvsendlist_v.rtnflowno = g_pub_tx.trace_no;

	g_reply_int = Mbfeghvsendlist_Upd_Upd(mbfeghvsendlist_v,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"查询业务回执对应的往账交易 执行游标数据库操作出错![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	Mbfeghvsendlist_Clo_Upd();

OkExit00:
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
