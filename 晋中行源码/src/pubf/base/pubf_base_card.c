/***************************************************************
* 文 件 名:     pubf_base_card.c
* 功能描述：    卡交易的公共函数
* 调用函数说明：
* 作    者:     ChenMing
* 完成日期：    2007年01月18日
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "seqctl_c.h"

/*************************************************
* 函 数 名:  iGetPreAuth_Traceno
* 功能描述： 生成预授权号
*            
* 作    者:  ChenMing
* 完成日期： 2007-1-17 
*************************************************/
long iGetPreAuth_Traceno()
{
	long lTrace_no;
	int ret=0;
	struct seqctl_c seqctl_c;
	memset(&seqctl_c, 0x00 , sizeof(seqctl_c));
	ret = Seqctl_Dec_Upd(g_pub_tx.reply," br_no='%s' and ac_ind='11'", TOT_BR_NO);
	if (ret)
	{
		sprintf(acErrMsg,"执行Seqctl_Dec_Upd错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	ret = Seqctl_Fet_Upd(&seqctl_c, g_pub_tx.reply);
	if (ret==100)
	{
		sprintf(acErrMsg,"执行Seqctl_Fet_Upd查询大额支付交易序号表错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf(acErrMsg,"执行seqctl_Fet_Upd 错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	lTrace_no = seqctl_c.seqno;
	if (seqctl_c.seqno==999999) seqctl_c.seqno=0;
	seqctl_c.seqno++;
	ret = Seqctl_Upd_Upd(seqctl_c, g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf(acErrMsg,"修改积分主表错误!");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	
	OkExit:
	    vtcp_log("取得支付交易序号=[%ld]成功![%s]", lTrace_no, g_pub_tx.reply);
	    return lTrace_no;
	ErrExit:
   	    vtcp_log("取得支付交易序号失败![%s]", g_pub_tx.reply);
		return -1;
}



