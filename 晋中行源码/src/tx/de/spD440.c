/***************************************************************
* 文 件 名: spD440.c
* 功能描述：晋中商行业务七天乐划款协议签约
* 作    者: 薛珂
* 完成日期：2011年4月8日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
insert into tx_def values('0008','七天乐签约','10100000000000000000100001000100000000100000110000000000000000000011100100000000000001000000000100000000000000000000000000100100','1','4','0');
insert into tx_flow_def values('0008',0,'D440','#=0000@1@#$');
insert into tx_flow_def values('0008',1,'D001','#=0000@2@#$');
insert into tx_flow_def values('0008',2,'D002','#=0000@3@#$');
insert into tx_flow_def values('0008',3,'D441','#$');
insert into tx_sub_def values('D440','七天乐划款协议签约','spD440','0','0');

insert into tx_sub_rel values('0008','D001','1','1',   '0191','启用0190');
insert into tx_sub_rel values('0008','D002','0','1167','1039','账户');
insert into tx_sub_rel values('0008','D002','0','1166','1038','户名');
insert into tx_sub_rel values('0008','D002','1','107', '1031','产品代码');
七天乐虚拟产品
insert into dd_parm
values ('107', '七天乐虚拟账户', 19850101, 99999999, '1', '01', 'D', '5', 'C', 0, 0, 0, 0, 'N', '000', 'Y', 'Y', '000', '000', 'C', '0', 0, 0, '100', '0', 'F', '000', 0, 0, '1', '2', 0, 0, '001', 'N', '000', '000', 0, 0, '000', 1, 'Y', 'Y', 'Y', '000', 'N', '000', 5, 'Y', 'Y', 'Y', 1, 'Y', 'Y', 'Y', 'Y', 'Y', 'N', 'Y', 'Y', 'N', 'N', '000', 0, 0, '2111', 'N');


*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "prdt_prot_parm_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "mo_prot_ac_rel_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

spD440()
{
    int  ret=0;
    char ac_no[20];
    char id_no[20];    
    char cif_type[2];
    char id_type[2];
    char draw_pwd[7];
    char   cAddr[60];
    char   cEmail[20];
    double dLeave_amt = 0.0;
        
    struct dd_mst_c  sDd_mst;                /*活期主张户表*/
    struct dd_parm_c  sDd_parm;              /*活期产品参数表*/
    struct mdm_ac_rel_c  sMdm_ac_rel;        /*介质与账户关系*/
    struct mo_prdt_prot_c  sMo_prdt_prot;    /*划款协议登记簿*/
    struct mo_prot_ac_rel_c  sMo_prot_ac_rel;/*划款协议账户对照表*/
    struct cif_addr_inf_c  sCif_addr_inf;
    struct cif_email_inf_c  sCif_email_inf;
    memset(cAddr,   0x00 , sizeof(cAddr));
    memset(cEmail,  0x00 , sizeof(cEmail));    
    memset(ac_no,   0x00 , sizeof(ac_no));
    memset(id_no,   0x00 , sizeof(id_no));
    memset(cif_type,0x00 , sizeof(cif_type));
    memset(id_type, 0x00 , sizeof(id_type));
    memset(draw_pwd,0x00 , sizeof(draw_pwd));
    
    memset(&sDd_parm,        0x00 , sizeof(struct dd_parm_c));
    memset(&sDd_mst,         0x00 , sizeof(struct dd_mst_c));
    memset(&sMdm_ac_rel,     0x00 , sizeof(struct mdm_ac_rel_c));
    memset(&sMo_prdt_prot,   0x00 , sizeof(struct mo_prdt_prot_c));
    memset(&sMo_prot_ac_rel, 0x00 , sizeof(struct mo_prot_ac_rel_c));
    memset(&sCif_addr_inf,   0x00 , sizeof(struct cif_addr_inf_c));
    memset(&sCif_email_inf,  0x00 , sizeof(struct cif_email_inf_c));
    
    /*** 初始化全局变量 ***/
    pub_base_sysinit();
     
     /*取值、赋值*/
    get_zd_data("0300", ac_no);              /*账号*/
    get_zd_data("0380", id_no);              /*证件号码*/
    get_zd_data("0260", cAddr);              /**地址**/
    get_zd_data("0330", cEmail);             /**电话**/
    get_zd_double("0410",&dLeave_amt);       /**留存金额**/
    pub_base_strpack(ac_no);
    pub_base_old_acno(ac_no); /* 对旧帐号的处理 */
    
    get_zd_data("0680", cif_type); /*客户类型*/
    get_zd_data("0890", id_type);  /*证件类型*/
    get_zd_data("0790", draw_pwd); /*密码*/
    
    /* 根据帐号查询账户序号和账户ID */
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",ac_no);
    if(ret)
      {
        sprintf(acErrMsg, "查询mdm_ac_rel错误!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"W011");
          goto ErrExit;
      }
    /**晋中,支持卡开七天乐..mdm_ac_rel.ac_seqn=9999 dd_mst.ac_seqn=1 20110511 去除ac_seqn限制.**/
    ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id = %ld ",sMdm_ac_rel.ac_id);
    if(ret == 100)
    {
        sprintf(acErrMsg, "无此记录!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D104");
          goto ErrExit;
    }
    else if(ret)
    {
        sprintf(acErrMsg, "数据库记录异常!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
          goto ErrExit;
    }
    /**add by zyl 20110511 七天乐金额限制**/
    vtcp_log("[%s][%d] dd_mst.bal=[%17.2f]",__FILE__,__LINE__,sDd_mst.bal);
    if( pub_base_CompDblVal(sDd_mst.bal-sDd_mst.ctl_amt-sDd_mst.hold_amt,50000.00+1000.00) < 0 )
    {
        sprintf(acErrMsg, "账户金额不足!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"A030");
          goto ErrExit;
    }
    /**end by zyl 20110511**/
    memset(&sMo_prdt_prot,   0x00 , sizeof(struct mo_prdt_prot_c));
    strcpy(sMo_prdt_prot.prot_type,"TK7");           /*协议类型*/
    sMo_prdt_prot.main_ac_id=sMdm_ac_rel.ac_id;      /*主账号*/
    sMo_prdt_prot.main_ac_seqn=sMdm_ac_rel.ac_seqn;  /*主账号序号*/
    strcpy(sMo_prdt_prot.tx_br_no,g_pub_tx.tx_br_no);/*登记机构编码*/
    sMo_prdt_prot.tx_date=g_pub_tx.tx_date;          /*登记日期*/
    sMo_prdt_prot.trace_no=g_pub_tx.trace_no;        /*主机流水号*/
    strcpy(sMo_prdt_prot.tel,g_pub_tx.tel);          /*登记柜员*/
    sMo_prdt_prot.sts[0]='1';                        /*状态: 1、正常；*、取消*/        
    sMo_prdt_prot.leave_amt=dLeave_amt;
    vtcp_log("[%s][%d] xk leave_amt=[%f]",__FILE__,__LINE__,sMo_prdt_prot.leave_amt);
    sMo_prdt_prot.turn_amt=50000;                    /*划转金额*/
    
    Mo_prdt_prot_Debug(&sMo_prdt_prot);
    
    ret = Mo_prdt_prot_Ins(sMo_prdt_prot,g_pub_tx.reply);
    if( ret )
     {
       sprintf(acErrMsg, "插入mo_prdt_prot错误[%d]!!",ret);
       WRITEMSG
       strcpy(g_pub_tx.reply,"D105");
         goto ErrExit;
     }
     
    /* pub_mob_sendmail("七天通知存款账户签约", g_pub_tx.tx_code,sMdm_ac_rel.ac_no,g_pub_tx.add_ind,0.0, 0.0 ); mod by lwb 20150519*/
     pub_mob_sendmail("七天通知存款账户签约", g_pub_tx.tx_code,sMdm_ac_rel.ac_no,"1",0.0, 0.0 ); 
     
    /**更改.客户基本信息.电话,地址. 若原来不存在记录.添加..by zyl 20110701**/
    /**cif_addr_inf update**/
    ret = sql_count("cif_addr_inf","cif_no=%ld",sDd_mst.cif_no);
    if(ret<0)
    {
        sprintf(acErrMsg,"调用sql_count出错!");
        WRITEMSG
        goto ErrExit;
    }
    else if(ret == 0)
    {
        sCif_addr_inf.cif_no = sDd_mst.cif_no;
        sCif_addr_inf.addr_seqn = 1;
        strcpy(sCif_addr_inf.addr_type,"2");
        strcpy(sCif_addr_inf.country,"01");
        strcpy(sCif_addr_inf.addr,cAddr);
        strcpy(sCif_addr_inf.addr,"");
        ret = Cif_addr_inf_Ins(sCif_addr_inf,g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"登记地址信息表错误![%d]",ret);
            WRITEMSG
            	strcpy(g_pub_tx.reply,"C023");
            goto ErrExit;
        }
    }
    else
    {
        ret = Cif_addr_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld ",sDd_mst.cif_no);
        if (ret)
        {
            sprintf(acErrMsg,"DECLARE Cif_addr_inf_rel游标异常![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
              goto ErrExit;
        }
        ret = Cif_addr_inf_Fet_Upd(&sCif_addr_inf,g_pub_tx.reply);
        if (ret)
        {
           	sprintf(acErrMsg,"FETCH Cif_addr_inf游标异常![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D102");
              goto ErrExit;
        }
        
        strcpy(sCif_addr_inf.addr,cAddr);
        ret = Cif_addr_inf_Upd_Upd(sCif_addr_inf,g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"UPDATE Cif_addr_inf游标异常![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D105");
              goto ErrExit;
        }
        Cif_addr_inf_Clo_Upd( );
    }
    /**cif_email_inf update**/
    ret = sql_count("cif_email_inf","cif_no=%ld",sDd_mst.cif_no);
    if(ret<0)
    {
        sprintf(acErrMsg,"调用sql_count出错!");
        WRITEMSG
        goto ErrExit;
    }
    else if(ret == 0)
    {
        sCif_email_inf.cif_no = sDd_mst.cif_no;
        sCif_email_inf.addr_seqn = 1;
        strcpy(sCif_email_inf.addr_type,"1");
        strcpy(sCif_email_inf.email,cEmail);
        strcpy(sCif_email_inf.link_man,sDd_mst.name);
        ret = Cif_email_inf_Ins(sCif_email_inf,g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"登记电子地址信息表错误![%d]",ret);
            WRITEMSG
            	strcpy(g_pub_tx.reply,"C024");
            goto ErrExit;
        }
    }
    else
    {
        ret = Cif_email_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld ",sDd_mst.cif_no);
        if (ret)
        {
            sprintf(acErrMsg,"DECLARE Cif_email_inf_rel游标异常![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
              goto ErrExit;
        }
        ret = Cif_email_inf_Fet_Upd(&sCif_email_inf,g_pub_tx.reply);
        if (ret)
        {
           	sprintf(acErrMsg,"FETCH Cif_email_inf游标异常![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D102");
              goto ErrExit;
        }
        strcpy(sCif_email_inf.email,cEmail);
        ret = Cif_email_inf_Upd_Upd(sCif_email_inf,g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"UPDATE Cif_email_inf游标异常![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D105");
              goto ErrExit;
        }
        Cif_email_inf_Clo_Upd( );
    }
    /**end update.**/
    set_zd_data("0192", "0");              /*不核对介质号码*/
    get_zd_data("0192", g_pub_tx.ac_wrk_ind);
    vtcp_log("%s,%d  seeeee hs=[%s]",__FILE__,__LINE__,g_pub_tx.ac_wrk_ind);
    set_zd_int("0193",  "1000");                    /*svc_ind--1000开户*/
    set_zd_data("1161", "0000");                    /*介质代号*/
    set_zd_data("1164", sMdm_ac_rel.id_type);       /*证件类型*/
    set_zd_data("1165", sMdm_ac_rel.id_no);         /*证件号码*/
   /**delete by jk 20130304程序编写错误
    set_zd_data("1168", "00000000BBBBBBBB");        凭证号码*/
    set_zd_data("1166", sDd_mst.name);              /*户名*/
    set_zd_data("1031", "107");                     /*产品代码*/
    set_zd_data("1033", "01");                      /*币种*/
    set_zd_data("1034", "5");                       /*账户类型*/
    set_zd_data("1036", "4");                       /*计息类型*/
    set_zd_data("1039", sMdm_ac_rel.ac_no);         /*账户*/
    vtcp_log("[%s][%d] zyl see3 opn_br_no=[%s]",__FILE__,__LINE__,sDd_mst.opn_br_no);
    set_zd_data("0020",sDd_mst.opn_br_no);/**20110427 虚拟账户的开户机构,不是交易机构.定为主账户的开户机构.**/
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "7天乐划款协议签约成功![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "7天乐划款协议签约失败![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
    
}
