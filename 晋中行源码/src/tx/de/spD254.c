/***************************************************************
* 文 件 名: spD254.c
* 功能描述: 大同城清算(本场挂账)
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

spD254()
{
    int ret;
    int no;
    char flag[2];
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
    /* 统计本场大同城提出金额 */
    ret = sql_sum_double("mo_city","bill_amt",&g_pub_tx.tx_amt1,
          "bill_type='4' and in_out_ind='1' and no=0");
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计本场大同城提出清算金额错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
    {
        goto OkExit1;
    }

    /* 修改本场次的场次号码 */
    ret = sql_max_int("mo_city","no","bill_type='4' and in_out_ind='1' and \
          tx_date=%ld", g_pub_tx.tx_date);
    if (ret < 0)
    {
        sprintf(acErrMsg,"取交易日期的场次最大值错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    no= ret +1;

    /* 修改本场次的场次号码 */
    ret = sql_execute("update mo_city set no =%d where bill_type='4' and \
          in_out_ind='1' and no = '0'",no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"修改本场次票据的场次号码错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 修改上场挂账金额 */
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    sprintf(com_parm_c.val,"%lf",g_pub_tx.tx_amt1);
    ret = sql_execute("update com_parm set val='%s' where parm_code='GZJE' \
          and parm_seqn=1",com_parm_c.val);
    if (ret != 0)
    {
        sprintf(acErrMsg,"修改上场挂账金额错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 本场次挂账 */
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 提出 借408外 */
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",g_pub_tx.tx_amt1);
    set_zd_data("1205","2");
    set_zd_data("120A","大同城清算记帐");

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=4");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 提出 贷科目408挂账 */
    set_zd_data("1211",com_parm_c.val);
    set_zd_double("1218",g_pub_tx.tx_amt1);
    set_zd_data("1215","2");
    set_zd_data("121A","大同城清算记帐");
    
	/*** 登记交易流水 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"大同城清算处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy(g_pub_tx.reply,"0000");
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

