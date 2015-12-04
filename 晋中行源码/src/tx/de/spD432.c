/*************************************************
* 文 件 名: spD432.c
* 功能描述：七天乐存款协议信息维护
* 作者    ：wanghongjie
* 时间    ：20110426
* insert into tx_def values       （1057 七天乐信息维护   10000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000100  1  1  2  3  ）
* insert into tx_flow_def values  （1057  0  D432  #$  ）
* insert into tx_sub_def values   （D432   七天乐信息维护   spD432  0  0  ）
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "mo_prdt_prot_c.h"
#include "mdm_ac_rel_c.h"

static struct mo_prdt_prot_c    g_mo_prdt_prot;
static struct mdm_ac_rel_c      g_mdm_ac_rel;

spD432()
{
    int ret=0;
    char acno[20];
    double amt=0.00;

    /** 初始化全局变量 **/
    pub_base_sysinit();
    
    /** 数据初始化 **/
    memset(&g_mo_prdt_prot, 0x00,sizeof(struct mo_prdt_prot_c));
    memset(acno,0x00,sizeof(acno));
    memset(&g_mdm_ac_rel, 0x00,sizeof(struct mdm_ac_rel_c));
    
    /** 取值、赋值 **/
    get_zd_data("0300",acno);
    get_zd_double("0410",&amt);

    /***修改****/
    /* 根据主帐号查询主账户序号和账户ID */
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no='%s'", acno);
    if (ret == 100)   
    {
        sprintf(acErrMsg, "不存在该帐号!!ac_no=[%s]", acno);
        WRITEMSG
        strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    } 
    else if (ret)    
    {
        sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错误!ret=[%s]", ret);
        WRITEMSG
        goto ErrExit;
    }
    /*更新划款协议登记薄留存金额*/
    ret=Mo_prdt_prot_Dec_Upd(g_pub_tx.reply,"main_ac_id=%ld ",g_mdm_ac_rel.ac_id);
    if (ret)
    {
        sprintf(acErrMsg,"DECLARE Mo_prdt_prot游标异常![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C006");
        ERR_DEAL;
    }
    
    ret=Mo_prdt_prot_Fet_Upd(&g_mo_prdt_prot,g_pub_tx.reply);
    if (ret)
    {
        sprintf(acErrMsg,"FETCH Mo_prdt_prot游标异常![%s]", g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C006");
        ERR_DEAL;
    }
    g_mo_prdt_prot.leave_amt=amt;
  
    ret=Mo_prdt_prot_Upd_Upd(g_mo_prdt_prot,g_pub_tx.reply);
    if (ret)
    {
        sprintf(acErrMsg,"UPDATE Mo_prdt_prot游标异常![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C006");
        ERR_DEAL;
    }
    Mo_prdt_prot_Clo_Upd( );
    
    /* 登记交易流水 */
    if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"登记交易流水 PASS!");
    WRITEMSG
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"七天乐存款协议信息维护成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"七天乐存款协议信息维护失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
