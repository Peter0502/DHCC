/***************************************************************
* 文 件 名: spD255.c
* 功能描述: 大同城清算(上场划回)
*
* 作    者: jack
* 完成日期: 2004年4月02日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

spD255()
{
    int ret;
    struct com_parm_c com_parm_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    g_pub_tx.tx_amt1 =0.00;
    /* 取上场提出挂账金额 */
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='GZJE' and \
        parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    g_pub_tx.tx_amt1 = atof(com_parm_c.val);
    sprintf(acErrMsg,"大同城清算上场挂账金额=[%d]",g_pub_tx.tx_amt1);
    WRITEMSG

    if (pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00) == 0)
    {
        goto OkExit1;
    }

    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 提入 贷408外 */
    set_zd_data("1211",com_parm_c.val);
    set_zd_double("1218",g_pub_tx.tx_amt1);
    set_zd_data("1215","2");
    set_zd_data("121A","大同城清算记帐");

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=4");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 提出 借科目408挂账 */
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",g_pub_tx.tx_amt1);
    set_zd_data("1205","2");
    set_zd_data("120A","大同城清算记帐");
    
	/*** 登记交易流水 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"大同城清算处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"大同城清算处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"大同城清算处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

