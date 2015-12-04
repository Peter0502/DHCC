/*************************************************************
* 文 件 名: sp0009.c
* 功能描述:根据活期帐号 返回客户信息
*
* 作    者: 薛珂
* 完成日期: 2011年4月2日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
insert into tx_def values('0009','返回客户信息','10000000000010000000000011000000100000001100000000000000001001000000000000000000100000001000000000000000000000000000000000000100','0','4','0');
insert into tx_flow_def values('0009',0,'0009','#$');
insert into tx_sub_def values('0009','返回客户信息','sp0009','0','0');
**************************************************************/
#define ERR_DEAL if( ret ) {\
        sprintf( acErrMsg, "sql error" ); \
        WRITEMSG \
        goto ErrExit; \
        }
#define TABLE_RECORD 4
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "mdm_attr_c.h"
#include "dd_parm_c.h"
#include "cif_addr_inf_c.h"
#include "cif_email_inf_c.h"


sp0009()
{
    int ret=0;
    char tx_br_no[6];
    
    struct mdm_attr_c     sMdm_attr;
    struct dd_mst_c       sDd_mst;
    struct dd_parm_c      sDd_parm;
    struct mdm_ac_rel_c   sMdm_ac_rel;
    struct com_sys_parm_c sCom_sys_pram;
    struct mo_prdt_prot_c sMo_prdt_prot;
    struct cif_addr_inf_c sCif_addr_inf;
    struct cif_email_inf_c sCif_email_inf;
    
    memset(&sCif_addr_inf,0x00 , sizeof(struct cif_addr_inf_c));
    memset(&sCif_email_inf,0x00, sizeof(struct cif_email_inf_c));
    memset(tx_br_no,      0x00 , sizeof(tx_br_no));
    memset(&sDd_parm,     0x00 , sizeof(struct dd_parm_c));
    memset(&sDd_mst,      0x00 , sizeof(struct dd_mst_c));
    memset(&sMdm_attr,    0x00 , sizeof(struct mdm_attr_c));
    memset(&sMdm_ac_rel,  0x00 , sizeof(struct mdm_ac_rel_c));
    memset(&sCom_sys_pram,0x00 , sizeof(struct com_sys_parm_c));
    memset(&sMo_prdt_prot,0x00 , sizeof(struct mo_prdt_prot_c));
  
    ret = sql_begin();    /* 打开事务 */
    if( ret ) 
    {
        sprintf(acErrMsg, "打开事务失败!!!");
        WRITEMSG
        goto ErrExit;
    }    
    /*初始化共用结构*/
    pub_base_sysinit();
    
    get_zd_data("0300",sMdm_ac_rel.ac_no);/*帐号*/
    ret = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_pram, "1=1" );
    if( ret )
    {
        sprintf(acErrMsg, "Com_sys_parm_Sel error ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D103");
        goto ErrExit;
    }
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",sMdm_ac_rel.ac_no);
    if( ret == 100 )
    {
        sprintf(acErrMsg, "记录不存在.");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D104");
        goto ErrExit;

    }
    else if( ret )
    {
        sprintf(acErrMsg, "Mdm_ac_rel_Sel error ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D103");
        goto ErrExit;
    }
    
		set_zd_data("0860",  sMdm_ac_rel.opn_br_no);     /*开户行机构号 lzy 20141227*/
     
    ret=Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'",sMdm_ac_rel.mdm_code);
    if( ret )
    {
        sprintf(acErrMsg, "Mdm_attr_Sel error ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D103");
        goto ErrExit;
    }
    ret=Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%d", sMdm_ac_rel.ac_id);
    if( ret )
    {
        sprintf(acErrMsg, "Mdm_attr_Sel error ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D103");
        goto ErrExit;
    }
    /*开户机构
    get_zd_data("0030",tx_br_no);
    if(memcmp(sDd_mst.opn_br_no,tx_br_no,5))
    {
        sprintf(acErrMsg, "只能在开户行签约!!");
        WRITEMSG
          goto ErrExit;
    }*/
    /*判断该状态是否正常*/
    if(sDd_mst.ac_sts[0] != '1')
    {
        sprintf(acErrMsg, "该状态不正常，不允许签约!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B124");
          goto ErrExit;
    }
    vtcp_log("[%s][%d] dd_mst.cif_no=[%ld]",__FILE__,__LINE__,sDd_mst.cif_no);
    /**add by zyl 20110701 只能是个人户.**/
    if(sDd_mst.cif_no > 20000000)
    {
        sprintf(acErrMsg, "该状态不正常，不允许签约!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O164");
          goto ErrExit;
    }
    /**end by zyl 20110701**/
    /**add by zyl 20110511 七天乐金额限制**/
    vtcp_log("[%s][%d] dd_mst.bal=[%17.2f]",__FILE__,__LINE__,sDd_mst.bal);
    if((sDd_mst.bal-sDd_mst.ctl_amt-sDd_mst.hold_amt) < (50000.00+1000.00) )
    {
        sprintf(acErrMsg, "账户金额不足!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"A030");
          goto ErrExit;
    }
    /**end by zyl 20110511**/
    ret=Dd_parm_Sel(g_pub_tx.reply, &sDd_parm, "prdt_no='%s'", sDd_mst.prdt_no);
    if( ret )
    {
        sprintf(acErrMsg, "Dd_parm_Sel error ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D103");
        goto ErrExit;
    }
    ret=Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts='1'", sDd_mst.ac_id);
    if( ret==0 )
    {
        sprintf(acErrMsg, "已存在此记录!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B360");
          goto ErrExit;
    }
    ret=Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts='2'", sDd_mst.ac_id);
    if( ret==0 )
    {
        sprintf(acErrMsg, "该状态为临时解约!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"L016");
          goto ErrExit;
    }

     vtcp_log("[%s][%d]look name=[%s]", __FILE__, __LINE__, sDd_mst.name);
     vtcp_log("[%s][%d]look bal=[%f]", __FILE__, __LINE__, sDd_mst.bal);
     vtcp_log("[%s][%d]look title=[%s]", __FILE__, __LINE__, sDd_parm.title);
     vtcp_log("[%s][%d]look note_type=[%s]", __FILE__, __LINE__, sMdm_attr.note_type);
     vtcp_log("[%s][%d]look note_no=[%s]", __FILE__, __LINE__, sMdm_ac_rel.note_no);
     vtcp_log("[%s][%d]look id_no=[%s]", __FILE__, __LINE__, sMdm_ac_rel.id_no);
     vtcp_log("[%s][%d]look min_bal=[%f]", __FILE__, __LINE__, sDd_parm.min_bal);
     set_zd_data("0250",  sDd_mst.name);          /*户名*/
     set_zd_double("0420",sDd_mst.bal);           /*余额*/
     set_zd_data("0810",  sDd_parm.title);        /*产品名称 不能用13域.*/
     set_zd_data("0890",  sMdm_attr.note_type);   /*凭证类型*/
     set_zd_data("0590",  sMdm_ac_rel.note_no);   /*凭证号码*/
     set_zd_data("0620",  sMdm_ac_rel.id_no);     /*证件号码*/
     set_zd_double("0410",sDd_parm.min_bal);      /*最低余额*/

     
    /**查询电话,联系地址.若不存在电话,地址.可以过.**/
    vtcp_log("[%s][%d] dd_mst.cif_no=[%ld]",__FILE__,__LINE__,sDd_mst.cif_no);
    ret = Cif_addr_inf_Sel(g_pub_tx.reply,&sCif_addr_inf,"cif_no = %ld ",sDd_mst.cif_no);
    if(ret == 100)
    {
        sprintf(acErrMsg, "记录不存在.");
        WRITEMSG
        /**
        strcpy(g_pub_tx.reply, "D104");
        goto ErrExit;
        **/
    }
    else if(ret)
    {
        sprintf(acErrMsg, "Cif_addr_inf_Sel error! ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
        goto ErrExit;
    }
    ret = Cif_email_inf_Sel(g_pub_tx.reply,&sCif_email_inf,"cif_no = %ld ",sDd_mst.cif_no);
    if(ret == 100)
    {
        sprintf(acErrMsg, "记录不存在.");
        WRITEMSG
        /**
        strcpy(g_pub_tx.reply, "D104");
        goto ErrExit;
        **/
    }
    else if(ret)
    {
        sprintf(acErrMsg, "Cif_email_inf_Sel error! ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
        goto ErrExit;
    }
    vtcp_log("[%s][%d] sCif_addr_inf.addr=[%s]",__FILE__,__LINE__,sCif_addr_inf.addr);
    vtcp_log("[%s][%d] sCif_email_inf.email=[%s]",__FILE__,__LINE__,sCif_email_inf.email);
    set_zd_data("0260",  sCif_addr_inf.addr);           /*地址*/
    set_zd_data("0330",  sCif_email_inf.email);         /*电话*/


OkExit:
    sql_commit();    /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"查询成功!!!!![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sql_rollback();    /*--事务回滚--*/
    sprintf(acErrMsg,"查询失败!!!!![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

