/***************************************************************
* 文 件 名:  spD316.c
* 功能描述:  遂心存解约	
* 作    者:  zqbo

* 完成日期：    
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_sxc_c.h"
#include "mo_sxc_hst_c.h"
#include "com_item_c.h"
#include "dc_acc_rel_c.h"

int spD316()
{
    int iRet=0;
    double dIntst = 0.00;
    
    char cFlag[2];
    
    struct mo_sxc_c      sMo_sxc;
    struct mdm_ac_rel_c  sMdm_ac_rel;
    struct com_item_c    sCom_item;
    struct dc_acc_rel_c   sDc_acc_rel;
    
    memset(cFlag,        0x00, sizeof(cFlag));
    memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
    memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
    memset(&sCom_item,   0x00, sizeof(sCom_item));
    memset(&sDc_acc_rel, 0x00, sizeof(sDc_acc_rel));
    /* 打开事务 */
    iRet = sql_begin();    
    if ( iRet )
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    pub_base_sysinit();
    
    get_zd_data("0300" ,g_pub_tx.ac_no);
    pub_base_strpack(g_pub_tx.ac_no);
    get_zd_int("0490",  &g_pub_tx.ac_seqn);
    get_zd_data("0720", cFlag);  /****S为销户解约专用，其他交易不可传值S****/
    
    /**销户解约时，将g_pub_tx.ac_seqn置为1 add by lq 20150915**/
    if(cFlag[0]=='S')
    	g_pub_tx.ac_seqn=1;
    /**end lq 20150915**/
    
    /***查询账户信息***/
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Mdm_ac_rel_Sel出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    /***-----------查找遂心存登记簿----------**/
    vtcp_log("[%s][%d] aaaaaaaaaaaa看看走到这里没有6,[%ld][%s][%d]", __FILE__, __LINE__, sMo_sxc.ac_id, cFlag, g_pub_tx.ac_seqn);
    iRet = Mo_sxc_Sel(g_pub_tx.reply, &sMo_sxc, "ac_id=%ld and ac_seqn=%d and sts='1'", sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn);
    if(iRet != 100 && iRet != 0)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    else if(iRet == 100)
    {
        if(cFlag[0] == 'S') /**cFlag=S-销户解约**/
        {
            vtcp_log( acErrMsg, "该账户没有签约，正常退出" );
            goto OkExit;
        }
        else
        {
            vtcp_log( acErrMsg, "该活期账户没有签约不需要解约！" );
            WRITEMSG
            strcpy(g_pub_tx.reply, "S035");
            goto ErrExit;
        }
    }
    
    vtcp_log("[%s][%d] aaaaaaaaaaaa看看走到这里没有4,[%ld]", __FILE__, __LINE__, sMo_sxc.ac_id);
    /**----------解约前滚积数----------**/
    g_pub_tx.ac_id = sMdm_ac_rel.ac_id;
    g_pub_tx.tx_amt1 = 0.00;
    strcpy( g_pub_tx.add_ind, "1" );  /**增加**/;
    strcpy( g_pub_tx.cur_no, "01" );
    strcpy( g_pub_tx.ct_ind, "2" );
    strcpy( g_pub_tx.prt_ind, "0" );  /*是否打折标志*/  
    g_pub_tx.svc_ind = 1001;          /**存取款***/
    strcpy(g_pub_tx.hst_ind, "1");
    strcpy(g_pub_tx.brf, "遂心存解约");
    strcpy(g_pub_tx.ac_wrk_ind, "000000000");
    g_reply_int = pub_acct_trance();
    if(g_reply_int != 0)
    {
        vtcp_log("滚动积数失败!");
        goto ErrExit;
    }
    
    vtcp_log("[%s][%d] aaaaaaaaaaaa看看走到这里没有3,[%ld]", __FILE__, __LINE__, sMo_sxc.ac_id);
    /***--------释放算息并登记结息明细--------***/
    iRet = iSxc_intst(sMdm_ac_rel.ac_id, sMo_sxc.ac_seqn, sMo_sxc.bal, &dIntst, '4');
    if(iRet)
    {
        sprintf( acErrMsg, "iSxc_intst出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /**------------更新登记簿-------------**/ 
    vtcp_log("[%s][%d] aaaaaaaaaaaa看看走到这里没有2,[%ld]", __FILE__, __LINE__, sMo_sxc.ac_id);
    dIntst += sMo_sxc.un_intst;
    sMo_sxc.intst += dIntst;
    vtcp_log("[%s][%d] aaaaaaaaaaaa看看走到这里没有1,[%ld]", __FILE__, __LINE__, sMo_sxc.ac_id);
    iRet = sql_execute("update mo_sxc set intst=%lf, untx_date=%ld, untrace_no=%ld, untx_br_no='%s', untx_tel='%s', bal=0.00, un_intst=0.00, sts='*' \
                        where ac_id=%ld and ac_seqn=%d and sts='1'", sMo_sxc.intst, g_pub_tx.tx_date, g_pub_tx.trace_no, g_pub_tx.tx_br_no, \
                        g_pub_tx.tel, sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /***------检测是否全部释放结息----------**/
    iRet = sql_count("mo_sxc_hst","ac_id=%ld and ac_seqn=%d and sts='1'", sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn);
    if(iRet > 0)
    {
        sprintf( acErrMsg, "解约结息出错!!!" );
        WRITEMSG
        goto ErrExit;	
    }
    /***登记trace_log**/
    g_pub_tx.ac_wrk_ind[1] = '1';
    if(cFlag[0] == 'S')
    {
    	get_zd_data("0680", g_pub_tx.ct_ind);
        g_pub_tx.tx_amt1 = dIntst; /**销户时显示结出利息**/
        strcpy(g_pub_tx.brf, "遂心存解约结息");
    }
    iRet = pub_ins_trace_log();
    if(iRet)
    {
        sprintf( acErrMsg, "登记trace_log错误!!" );
        WRITEMSG
        goto ErrExit;	
    }
    
    /***--------传值-------**/
    strcpy(g_pub_tx.opn_br_no, g_dd_mst.opn_br_no);
    set_zd_double("100E", dIntst);  
    set_zd_data("100A", "sx");    /**2204中D202用**/
    if( cFlag[0] != 'S') /**解约交易用，非销户自动解约**/
    {
    	/**取活期利息支出科目**/
    	iRet = Dc_acc_rel_Sel(g_pub_tx.reply, &sDc_acc_rel, "dc_code='%s' and data_code='0153'", g_dd_parm.dc_code);
    	if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Dc_acc_rel_Sel出错!!!" );
            WRITEMSG
            goto ErrExit;
        }
    	iRet = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_hrt='%s' ", sDc_acc_rel.acc_hrt);
    	if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Com_item_Sel出错!!!" );
            WRITEMSG
            goto ErrExit;
        }
        set_zd_data("1201", sCom_item.acc_no);
    	set_zd_data("120A", "遂心存解约结息");
        set_zd_data("1016", "遂心存解约");
        set_zd_data("0620", "遂心存解约结息");
    }
     
OkExit:
	/*--提交事务--*/
    sql_commit();  
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "success!!");
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
    
ErrExit:
	/*--事务回滚--*/
    sql_rollback();
    sprintf(acErrMsg, "failed !! reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    if(strlen(g_pub_tx.reply)==0 || strcmp(g_pub_tx.reply, "0000")==0)
    {
        strcpy(g_pub_tx.reply, "S035");	
    }
    return 1;

}
