/*************************************************
* 文 件 名:  spB506.c
* 功能描述： 同业拆出回收
*
* 作    者:  rob
* 完成日期： 2003年1月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_c.h"

int spB506()
{
    struct mo_ibl_c sMo_ibl_c;
    struct mdm_ac_rel_c sMdm_ac_rel;
    double  bal;
    char ac_no[20];
    int ret;
	long trace_no;
  
    memset(&sMo_ibl_c,0x0,sizeof(struct mo_ibl_c));
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
    memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0730",sMo_ibl_c.pact_no);      /*合同号*/
    get_zd_double("0420",&g_pub_tx.tx_amt1);    /*还款金额*/
    get_zd_double("0410",&bal);                 /*拆借账户余额*/

    
    /* 修改拆借登记簿状态 */
    ret = Mo_ibl_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sts = '2' and \
          sign = '2'", sMo_ibl_c.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_ibl_Fet_Upd(&sMo_ibl_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"fecth for update error !! [%d]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        goto ErrExit;
    }

    /* 检查拆借账号输入是否和拆借登记簿中账户一致 *
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",
          sMdm_ac_rel.ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询介质和账户关系表错误!! [%d]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"W011");
        goto ErrExit;
    }
    if (sMdm_ac_rel.ac_id != sMo_ibl_c.ac_id)
    {
        sprintf(acErrMsg,"拆借账号和登记簿中的登记账号不同");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B078");
        goto ErrExit;
    }
	*****/

    /* 检查金额是否大于未还款金额 */
    if (pub_base_CompDblVal(bal,g_pub_tx.tx_amt1) == -1)
    {
        sprintf(acErrMsg,"还款金额大于登记簿中的未还款金额!!");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B079");
        goto ErrExit;
    }

    g_pub_tx.hst_ind[0] = '1';
	g_pub_tx.ac_id = sMo_ibl_c.ac_id;
	g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.brf,"同业拆出回收");

	if (pub_base_CompDblVal(bal, g_pub_tx.tx_amt1) == 0)
    {
        /* 销户 */
        g_pub_tx.add_ind[0] = '1';
		g_pub_tx.svc_ind = 1003;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"销户出错 ");
            WRITEMSG
            Mo_ibl_Clo_Upd();
            goto ErrExit;
        }

        sprintf(acErrMsg,"拆借账户销户成功!!!! pass!!");
        WRITEMSG
       
        strcpy(sMo_ibl_c.sts,"*");
        sMo_ibl_c.no_pay_bal = bal - g_pub_tx.tx_amt1;

    }
    else
    {
        /* 向拆借账户中取款 */
        g_pub_tx.add_ind[0] = '1';
        g_pub_tx.ac_seqn = 0;
		g_pub_tx.svc_ind = 1001;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"向拆借账户存款错误");
            WRITEMSG
            Mo_ibl_Clo_Upd();
            goto ErrExit;
        }

        sMo_ibl_c.no_pay_bal = bal - g_pub_tx.tx_amt1;

        sprintf(acErrMsg,"拆借账户记账成功!!!! pass!!");
        WRITEMSG
    }

	trace_no = sMo_ibl_c.trance_no;
	sMo_ibl_c.trance_no = g_pub_tx.trace_no;

    ret = Mo_ibl_Upd_Upd(sMo_ibl_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新拆借登记簿错误!! ret =[%s]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B008");
        goto ErrExit;
    }
    Mo_ibl_Clo_Upd();

    sprintf(acErrMsg," PASS!!!!!");
    WRITEMSG

	/******
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }
	********/

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

