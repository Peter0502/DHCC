/***************************************************************
* 文 件 名: gD069.c
* 功能描述: 网点提入销账(日终)
*
* 作    者: mike
* 完成日期: 2003年4月04日
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

gD069()
{
    int ret;
	int flag;
	double amt1,amt2;
    struct com_parm_c com_parm_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    /* 统计金额 */
	/* 提入借方 */
    ret = sql_sum_double("mo_city","bill_amt",&amt1,"in_out_ind = '2' and \
			cd_ind = '1' and (sts='A' or sts='H')");
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计同城提入挂账金额错误 [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"提入借方金额=[%lf]",amt1);
    WRITEMSG

	/* 提入贷方 */
    ret = sql_sum_double("mo_city","bill_amt",&amt2,"in_out_ind = '2' and \
			cd_ind = '2' and (sts='A' or sts='H')");
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计同城提入挂账金额错误 [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"提入贷方金额=[%lf]",amt1);
    WRITEMSG

	if (pub_base_CompDblVal(amt2,amt1) > 0)
	{
		g_pub_tx.tx_amt1 = amt2 - amt1;
		flag = 2;
	}
	else if (pub_base_CompDblVal(amt2,amt1) < 0)
	{
		g_pub_tx.tx_amt1 = amt2 - amt1;
		flag = 1;
	}
	else
	{
		goto OkExit;
	}

    sprintf(acErrMsg,"g_pub_tx.tx_amt=[%lf]",g_pub_tx.tx_amt1);
    WRITEMSG

    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	if (flag == 1)
	{
    	/* 提入 借408外 */
    	set_zd_data("0210","01");
    	set_zd_data("0152",com_parm_c.val);
    	set_zd_double("0390",g_pub_tx.tx_amt1);
	}
	else
	{
    	/* 提入 贷408外 */
    	set_zd_data("0210","01");
    	set_zd_data("0155",com_parm_c.val);
    	set_zd_double("0390",g_pub_tx.tx_amt1);
	}

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=6");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	if (flag == 1)
	{
        /* 提入 贷408挂账科目 */
    	set_zd_data("0155",com_parm_c.val);
	}
	else
	{
        /* 提入 借408挂账科目 */
    	set_zd_data("0152",com_parm_c.val);
	}

    ret = pubf_acct_proc("g066");
    if( ret )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
    }

    
	/*** 登记交易流水 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"网点提入销帐成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"网点提入销帐失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

