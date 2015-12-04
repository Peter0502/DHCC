/***************************************************************
* 文 件 名:     pubf_acct_cls_mdm.c
* 功能描述：    销空介质
*               1、修改介质账户对照表；
*               2、登记交易流水；
* 入口参数：
* 作    者:     jack
* 完成日期：    2003年1月16日
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
#include "mdm_attr_c.h"
#include "mdm_unprt_hst_c.h"

pub_acct_cls_mdm()
{
    struct mdm_attr_c sMdm_attr;
    struct mdm_unprt_hst_c sMdm_unprt_hst;
    g_reply_int=0;

    g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s' ",g_pub_tx.ac_no);
    if( g_reply_int )
    {
        sprintf(acErrMsg,"账号不存在[%s][%d]",g_pub_tx.ac_no,g_reply_int);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
    Mdm_ac_rel_Debug(&g_mdm_ac_rel);
    if( g_reply_int==100 )
    {
        sprintf(acErrMsg,"显示账号不存在[%s]",g_pub_tx.ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    else if( g_reply_int )
    {
        sprintf(acErrMsg,"账号不存在[%s][%d]",g_pub_tx.ac_no,g_reply_int);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
    strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);
    strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
    if( g_pub_tx.ac_wrk_ind[0]!='0' && strlen(g_mdm_ac_rel.note_no) && strcmp(g_pub_tx.beg_note_no,g_mdm_ac_rel.note_no) )
    {
        sprintf(acErrMsg,"凭证号不符![%s][%s][%s]",g_pub_tx.beg_note_no,g_mdm_ac_rel.note_no,g_pub_tx.ac_wrk_ind);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P169");
        goto ErrExit;
    }

    vtcp_log("[%s][%d]  NOTE_STS==[%s][%c] TX_CODE==[%s]",__FILE__,__LINE__,\
             g_mdm_ac_rel.note_sts,g_mdm_ac_rel.note_sts[0],g_pub_tx.tx_code);
    switch( g_mdm_ac_rel.note_sts[0] )
    {
    case '1':
        /*增加扣划特殊处理 挂失允许销户 2011/11/13 0:42:40*/
        if(memcmp(g_pub_tx.tx_code,"4713",4) != 0 \
        && memcmp(g_pub_tx.tx_code,"2204",4) != 0 )/*** 对储蓄销户不加限制 折子也能挂失销户 20130702 ***/
        {
            sprintf(acErrMsg,"该介质已经被挂失![%s]",g_mdm_ac_rel.note_sts);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P170");
            goto ErrExit;
        }
        break;
    case '2':
        sprintf(acErrMsg,"该介质已经被挂失换证![%s]",g_mdm_ac_rel.note_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P171");
        goto ErrExit;
    case '*':       
        sprintf(acErrMsg,"该介质已经销户![%s]",g_mdm_ac_rel.note_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P172");
        goto ErrExit;
    default:
        break;
    }
    strcpy(g_mdm_ac_rel.note_sts,"*");
    
    if( g_mdm_ac_rel.coll_sts[0]=='1' )
    {
        sprintf(acErrMsg,"该介质已经被捡拾![%s]",g_mdm_ac_rel.coll_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W038");
        goto ErrExit;
    }

    /*** 检查支取方式函数 ***/
    if(memcmp(g_pub_tx.tx_code,"4713",4) != 0)
    {
        if( pub_base_check_draw("001") )
        {
            sprintf(acErrMsg,"检查支取方式错误!");
            WRITEMSG
            goto ErrExit;
        }
    }
    

    g_reply_int=sql_count("td_mst","ac_id=%ld and (ac_sts='1' or ac_sts='0')",g_pub_tx.ac_id);  
    if( g_reply_int!=0 )
    {
        sprintf(acErrMsg,"该介质还有定期账户没销![%d]",g_reply_int);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P173");
        goto ErrExit;
    }

    g_reply_int=sql_count("dd_mst","ac_id=%ld and (ac_sts='1' or ac_sts='0')",g_pub_tx.ac_id);  
    if( g_reply_int!=0 )
    {
        sprintf(acErrMsg,"该介质还有活期账户没销![%d]",g_reply_int);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P174");
        goto ErrExit;
    }

    /* add by xxx at 20030726 检查是否有未登明细 **/
    pub_base_strpack(g_mdm_ac_rel.mdm_code);
    g_reply_int = Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'", 
            g_mdm_ac_rel.mdm_code);
    if (g_reply_int != 0 && g_reply_int != 100)
    {
        sprintf(acErrMsg, "查询介质属性表错误!! [%d]", g_reply_int);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N056");
        goto ErrExit;
    }
    else if (g_reply_int == 100)
    {
        sprintf(acErrMsg, "介质属性表中不存在该记录!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N055");
        goto ErrExit;
    }

    if (sMdm_attr.prt_ind[0] == 'Y')
    {
        sprintf(acErrMsg, "检查未等折明细表!!");
        WRITEMSG
        g_reply_int = sql_count("mdm_unprt_hst", "ac_id=%ld and ac_seqn=%d",
            g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn);
        if (g_reply_int < 0)
        {
            sprintf(acErrMsg, "查询未登折明细表异常!! [%d]", g_reply_int);
            WRITEMSG
            strcpy(g_pub_tx.reply, "O239");
            goto ErrExit;
        }
        else if (g_reply_int > 0)
        {
            sprintf(acErrMsg, "未登折明细表中还有记录!! [%d]", g_reply_int);
            WRITEMSG
            strcpy(g_pub_tx.reply, "O240");
            goto ErrExit;
        }
    }
    /**** end *****/

    g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
    if( g_reply_int )
    {
        sprintf(acErrMsg,"账号不存在[%s][%d]",g_pub_tx.ac_no,g_reply_int);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    Mdm_ac_rel_Clo_Upd();

/**
    if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水出错!");
        WRITEMSG
        goto ErrExit;
    }
**/

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"销介质成功![%s]",g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
    sprintf(acErrMsg,"销介质失败![%s]",g_pub_tx.reply);
    WRITEMSG
    return 1;
}
