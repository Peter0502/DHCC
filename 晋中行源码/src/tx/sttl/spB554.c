/*************************************************
* 文 件 名:  spB554.c
* 功能描述： 电票： 贴现通知 
*            从平台接收数据，为贴现业务的录入和记账准备数据.
*
* 作    者: yindianheng
* 完成日期： 2010年4月6日
*
* 修改记录：
*   日    期: 2010-4-29 17:42:55
*   修 改 人: xyy
*   修改内容:
insert into tx_def values ('4954','贴现通知','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)
insert into tx_flow_def   values ('4954','0','B554','#=0001@1@#$')
insert into tx_flow_def   values ('4954','1','B911','#=0000@2@#$')
insert into tx_flow_def   values ('4954','2','B913','#=0000@3@#$')
insert into tx_flow_def   values ('4954','3','L104','#=0002@4@#$')
insert into tx_flow_def   values ('4954','4','NULL','#=0001@5@#=0002@6@#$')
insert into tx_flow_def   values ('4954','5','D099','#$')
insert into tx_flow_def   values ('4954','6','a017','#$')
insert into tx_sub_def    values ('B554','贴现通知','spB554','0','0') ;

insert into tx_sub_rel values ('4954','D099','1','1','0191','')
insert into tx_sub_rel values ('4954','D099','1','0000','0192','')
insert into tx_sub_rel values ('4954','D099','1','1001','0193','')
insert into tx_sub_rel values ('4954','D099','0','0630','1011','')
insert into tx_sub_rel values ('4954','D099','1','1','1012','')
insert into tx_sub_rel values ('4954','D099','0','1002-1001','1013','')
insert into tx_sub_rel values ('4954','D099','1','2','101B','')
insert into tx_sub_rel values ('4954','L104','1','0','0340','')
insert into tx_sub_rel values ('4954','L104','0','0330','0370','')
insert into tx_sub_rel values ('4954','L104','0','1002','0390','')
insert into tx_sub_rel values ('4954','L104','1','2','0660','')
insert into tx_sub_rel values ('4954','NULL','0','0670','0120','')
insert into tx_sub_rel values ('4954','a017','0','0630','1211','贴现通知')
insert into tx_sub_rel values ('4954','a017','1','01','1214','币种')
insert into tx_sub_rel values ('4954','a017','1','2','1215','转账')
insert into tx_sub_rel values ('4954','a017','0','0900','1216','凭证种类')
insert into tx_sub_rel values ('4954','a017','0','0600','1217','凭证号码')
insert into tx_sub_rel values ('4954','a017','0','1002-1001','1218','金额')
insert into tx_sub_rel values ('4954','a017','1','贴现通知','121A','摘要')
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "hv_define.h"
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "hv_orno_brno_c.h"
#include "hv_uniontab_c.h"
#include "mdm_ac_rel_c.h"
int spB554()
{
    int     ret=0;    
    char    cstlFlg[5];           /* 线上清算标志*/
    char    cpn_ac_no[33];        /* 贴出人账号  */
    char    cdscntTyp[5];         /* 贴现类型    */
    char    caoAcctSvcr[20];      /* 入账账号开户行 */
    char    cremitter[61];        /* 出票人名称  */
    char    cRem_ac_no[33];       /* 出票人账号  */
    char    cpayee_ac_no[33];     /* 入账账号    */
    char    cdscntInAcctSvcr[20]; /* 贴入行行号  */
    char    cPayee_brno_name[20]; /* 贴出人开户行*/
    char    cTmpAcno[21];         /* 记账账号    */
    char    cbuff[61];
    char    cKinbr[BRNO_LEN+1];
    char    lw_ind[2]; 
    
    double  ddrftAmt=0.0;         /* 票面金额*/
    double  ddrftRAmt=0.0;         /* 实付金额*/
    
    struct mdm_ac_rel_c sMdmAcRel ,sMdm_ac_rel;
    struct mo_discnt_c    sMoDiscnt;    /* 贴现登记薄 */
    struct ecds_recv_reg_c    sEcdsRecvReg,sEcdsRecvReg1;    
    struct hv_orno_brno_c     sHvOrnoBrno_in,sHvOrnoBrno_out;
    struct hv_uniontab_c     sHv_uniontab_in,sHv_uniontab_out,sHv_uniontab_t;
    
    memset(cbuff, 0x00, sizeof(cbuff)); 
    memset(cKinbr ,  0 , sizeof(cKinbr));
    memset(cstlFlg, 0x00, sizeof(cstlFlg));
    memset(cTmpAcno, 0  , sizeof(cTmpAcno));
    memset(cremitter, 0x00, sizeof(cremitter));
    memset(cpn_ac_no, 0x00, sizeof(cpn_ac_no));
    memset(cdscntTyp, 0x00, sizeof(cdscntTyp));
    memset(cRem_ac_no, 0x00, sizeof(cRem_ac_no));
    memset(caoAcctSvcr, 0x00, sizeof(caoAcctSvcr));
    memset(cpayee_ac_no, 0x00, sizeof(cpayee_ac_no));
    memset(cdscntInAcctSvcr, 0x00, sizeof(cdscntInAcctSvcr));
    memset(cPayee_brno_name, 0x00, sizeof(cPayee_brno_name));
    memset(lw_ind, 0x00, sizeof(lw_ind));
    
    memset(&sMdmAcRel , 0 , sizeof(struct mdm_ac_rel_c));
    memset(&sMdm_ac_rel , 0 , sizeof(struct mdm_ac_rel_c));
    memset(&sMoDiscnt, 0x00, sizeof(struct mo_discnt_c));
    memset(&sEcdsRecvReg, 0x00, sizeof(struct ecds_recv_reg_c));    
    memset(&sHvOrnoBrno_in, 0x00, sizeof(struct hv_orno_brno_c));
    memset(&sHvOrnoBrno_out, 0x00, sizeof(struct hv_orno_brno_c));
    memset(&sEcdsRecvReg1, 0x00, sizeof(sEcdsRecvReg1));
    memset(&sHv_uniontab_in, 0x00, sizeof(struct hv_uniontab_c));
    memset(&sHv_uniontab_t, 0x00, sizeof(struct hv_uniontab_c));
    memset(&sHv_uniontab_out, 0x00, sizeof(struct hv_uniontab_c));
    
    
    /* 数据初始化 */
    pub_base_sysinit();
    /****从平台得到基本信息****/
    get_zd_long("0460",&sEcdsRecvReg.ecds_tx_date);  /* 日期        */        
    get_zd_data("0340",cstlFlg);                     /* 线上清算标志*/   
     
    get_zd_data("0750", cbuff);                      /* 票据号码   */    
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.drftnb,cbuff,sizeof(sEcdsRecvReg.drftnb));
    memset(cbuff, 0x00, sizeof(cbuff)); 
    
    get_zd_long("0450", &sEcdsRecvReg.duedt);        /* 到期日期、 */
    get_zd_data("0300", cbuff);                  /* 贴出人账号 */
    zip_space(cbuff);
    memcpy(cpn_ac_no,cbuff,sizeof(cpn_ac_no));
    memset(cbuff, 0x00, sizeof(cbuff)); 
     
    get_zd_data("0310", cbuff);               /* 入账账号   */ 
    zip_space(cbuff);
    memcpy(cpayee_ac_no,cbuff,sizeof(cpayee_ac_no));
    memset(cbuff, 0x00, sizeof(cbuff));    
    
    get_zd_data("101F", cbuff); /* 交易流水号 */    
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.ecds_trace_no,cbuff,sizeof(sEcdsRecvReg.ecds_trace_no));
    memset(cbuff, 0x00, sizeof(cbuff)); 
     
    get_zd_data("0910", cbuff);       /* 票据类型 */
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.drfttyp,cbuff,sizeof(sEcdsRecvReg.drfttyp));
    memset(cbuff, 0x00, sizeof(cbuff)); 
     
    get_zd_data("0370", cbuff);           /* 贴入行行号 */
    zip_space(cbuff);
    memcpy(cdscntInAcctSvcr,cbuff,sizeof(cdscntInAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff)); 
        
    get_zd_data("0980", cbuff);           /* 来往标示 */
    zip_space(cbuff);
    memcpy(lw_ind,cbuff,sizeof(lw_ind));
    memset(cbuff, 0x00, sizeof(cbuff)); 
    
    get_zd_data("0380", cbuff);           /* 贴出人开户行 */    
    zip_space(cbuff);
    memcpy(cPayee_brno_name,cbuff,sizeof(cPayee_brno_name));
    memset(cbuff, 0x00, sizeof(cbuff)); 
    get_zd_data("0350",cdscntTyp);                   /* 贴现类型 */
    
    get_zd_data("0590",cbuff);                 /* 入账账号开户行 */
    zip_space(cbuff);
    memcpy(caoAcctSvcr,cbuff,sizeof(caoAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff)); 
    
    get_zd_data("0260",cremitter);                   /* 出票人名称 */
    
    get_zd_data("0330",cbuff);                  /* 出票人账号*/
    zip_space(cbuff);
    memcpy(cRem_ac_no,cbuff,sizeof(cRem_ac_no));
    memset(cbuff, 0x00, sizeof(cbuff)); 
    
    get_zd_double("0400",&ddrftAmt);                 /* 票面金额*/
    get_zd_double("0410",&ddrftRAmt);                 /* add by  ligl 20100505实付金额*/
    vtcp_log(" [%s][%d] 日期:[%ld]",__FILE__,__LINE__,sEcdsRecvReg.ecds_tx_date);
    vtcp_log(" [%s][%d] 线上清算标志:[%s]",__FILE__,__LINE__,cstlFlg);
    vtcp_log(" [%s][%d] 票据号码:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.drftnb);
    vtcp_log(" [%s][%d] 到期日期:[%ld] ",__FILE__,__LINE__,sEcdsRecvReg.duedt);
    vtcp_log(" [%s][%d] 贴出人账号:[%s] ",__FILE__,__LINE__,cpn_ac_no);
    vtcp_log(" [%s][%d] 入账账号:[%s] ",__FILE__,__LINE__,cpayee_ac_no);
    vtcp_log(" [%s][%d] 交易流水号:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.ecds_trace_no);
    vtcp_log(" [%s][%d] 票据类型:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.drfttyp);
    vtcp_log(" [%s][%d] 贴入行行号:[%s] ",__FILE__,__LINE__,cdscntInAcctSvcr);
    vtcp_log(" [%s][%d] 贴出人开户行:[%s] ",__FILE__,__LINE__,cPayee_brno_name);
    vtcp_log(" [%s][%d] 贴现类型:[%s] ",__FILE__,__LINE__,cdscntTyp);
    vtcp_log(" [%s][%d] 入账账号开户行:[%s] ",__FILE__,__LINE__,caoAcctSvcr);
    vtcp_log(" [%s][%d] 出票人名称:[%s] ",__FILE__,__LINE__,cremitter);
    vtcp_log(" [%s][%d] 出票人账号:[%s] ",__FILE__,__LINE__,cRem_ac_no);
    vtcp_log(" [%s][%d] 票面金额:[%f] ",__FILE__,__LINE__,ddrftAmt);
    
    
    zip_space(cpn_ac_no);
    
            
     
    /*-----------判断该电票是否已经处理过------------*/
    ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
            sEcdsRecvReg.ecds_tx_date, sEcdsRecvReg.ecds_trace_no);
    if(ret==0)
    {
        sprintf(acErrMsg,"Ecds_recv_reg该记录存在!! ret [%d],trace_no=[%s],date=[%ld] ",\
            ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
        WRITEMSG  
        if(sEcdsRecvReg1.bf_flag[0]!='1')
        {
                sprintf(acErrMsg,"错误,该记录已经处理过!! bf_flag [%s],trace_no=[%s],date=[%ld] ",\
                        sEcdsRecvReg1.bf_flag,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                WRITEMSG 
                strcpy(g_pub_tx.reply, "NXDP");       
                goto ErrExit; 
        }
        memset(&sEcdsRecvReg1, 0x00, sizeof(sEcdsRecvReg1));
        ret=Ecds_recv_reg_Dec_Upd(g_pub_tx.reply , "ecds_trace_no='%s' and ecds_tx_date= '%ld'",\
                sEcdsRecvReg.ecds_trace_no,sEcdsRecvReg.ecds_tx_date);
        if (ret) 
        {
            sprintf(acErrMsg, "错误，Ecds_recv_reg_Dec_Upd![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        ret=Ecds_recv_reg_Fet_Upd(&sEcdsRecvReg1,g_pub_tx.reply);
        if (ret) 
        {
            sprintf(acErrMsg, "错误，Ecds_recv_reg_Fet_Upd![%d]", ret);
            WRITEMSG
            goto ErrExit;
       }
       memcpy(sEcdsRecvReg1.bf_flag,"2",sizeof(sEcdsRecvReg1.bf_flag));    
       ret=Ecds_recv_reg_Upd_Upd(sEcdsRecvReg1,g_pub_tx.reply);
       if (ret)
       {
            sprintf(acErrMsg, "Ecds_recv_reg_Upd_Upd错![%d]", ret);
            WRITEMSG
            goto ErrExit;
       }
       Ecds_recv_reg_Clo_Upd();       
         
    }
    else if(ret==100)
    {
        vtcp_log(" [%s][%d] ret=[%d] Ecds_recv_reg没有查到相应的信息!!",__FILE__,__LINE__,ret);
        
        sEcdsRecvReg.trace_no=g_pub_tx.trace_no;    /* 核心流水号 */    
        sEcdsRecvReg.tx_date=g_pub_tx.tx_date;    /* 核心交易日期 */
        memcpy(sEcdsRecvReg.drftcls,"E",sizeof(sEcdsRecvReg.drftcls)-1);    /* 票据分类 */ 
        sEcdsRecvReg.issedt=0;    /* 出票日??--暂赋零 */     
        memcpy(sEcdsRecvReg.tag_name,"CBS1000060",sizeof(sEcdsRecvReg.tag_name)-1);    /* 报文类型 */
        sEcdsRecvReg.bf_flag[0]='0';
        memcpy(sEcdsRecvReg.sts,"1",sizeof(sEcdsRecvReg.sts)-1);    /* 接收状态 */
        memcpy(sEcdsRecvReg.check_flag,"2",sizeof(sEcdsRecvReg.check_flag)-1);/*对账标志*/
        ret=Ecds_recv_reg_Ins(sEcdsRecvReg,g_pub_tx.reply);    
        if (ret) 
        {
              sprintf(acErrMsg, "插入Ecds_recv_reg错![%d]", ret);
              WRITEMSG
              goto ErrExit;
       }
       Ecds_recv_reg_Debug(&sEcdsRecvReg); 
    }
    else
    {
        sprintf(acErrMsg,"[%s][%d] 查询Ecds_recv_reg表错误!! ret [%d]",__FILE__,__LINE__,ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
       
  
    /*---------判断转换票据类型-------------------*/
    vtcp_log("[%s][%d] 票据类型:[%s]",__FILE__,__LINE__,sEcdsRecvReg.drfttyp);
    if(!memcmp(sEcdsRecvReg.drfttyp,"AC01",4))
    {
        memcpy(sEcdsRecvReg.drfttyp,"111",4);
    }
    else if(!memcmp(sEcdsRecvReg.drfttyp,"AC02",4))
    {
        memcpy(sEcdsRecvReg.drfttyp,"112",4);        
    }
    else
    {
        sprintf(acErrMsg, "票据类型错误!! ");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
       vtcp_log("[%s][%d]--------线上清算：[%s]",__FILE__,__LINE__,cstlFlg);
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_in," or_br_no=%s ",cdscntInAcctSvcr);
        if (ret)
        {
            sprintf(acErrMsg, "[贴入]查询Hv_uniontab表错误!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_out," or_br_no=%s ",caoAcctSvcr);/**TODO 这里需要确认是贴入人开户行还是入账开户行**/
        if (ret)
        {
            sprintf(acErrMsg, "[贴出]查询Hv_uniontab表错误!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
    if(lw_ind[0]=='1')
    {
    /* -----------检查贴现登记薄中，是否和平台传过来的数一致-----------  */
    ret = Mo_discnt_Sel(g_pub_tx.reply, &sMoDiscnt, "pnote_no='%s' and not_matr_date='%ld'  and note_type='%s' and sts ='X'", \
            sEcdsRecvReg.drftnb, sEcdsRecvReg.duedt,sEcdsRecvReg.drfttyp);
    if (ret ==100) {
        sprintf(acErrMsg, "贴现登记薄中不存在此票据类型和票据号!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B174");
        goto ErrExit;
    } else if (ret) {
        sprintf(acErrMsg, "执行Mo_discnt_Sel错![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    if(pub_base_CompDblVal(sMoDiscnt.par_amt,ddrftAmt)!=0)
    {
        vtcp_log("[%s][%d] par_amt=[%f] drftAmt =[%f]",__FILE__,__LINE__,sMoDiscnt.par_amt,ddrftAmt);
        sprintf(acErrMsg,"检查平台送来的票面金额错误!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B103" );
        goto ErrExit;
    }
    if(memcmp(sMoDiscnt.rem_ac_no,cRem_ac_no ,sizeof(sMoDiscnt.rem_ac_no)-1)!=0)
    {
        vtcp_log("[%s][%d] rem_ac_no=[%s] cRem_ac_no =[%s]",__FILE__,__LINE__,sMoDiscnt.rem_ac_no,cRem_ac_no);
        sprintf(acErrMsg,"检查平台送来的出票人账号错误!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B103" );
        goto ErrExit;
    }
    
    GetBr_noByOr_br_no(cdscntInAcctSvcr,cKinbr);
    vtcp_log("[%s][%d] cKinbr=[%s][%s] ",__FILE__,__LINE__,cKinbr,sMoDiscnt.br_no);

    if(memcmp(cKinbr,sMoDiscnt.br_no,BRNO_LEN)!=0)
    {
        sprintf(acErrMsg,"非我行票据=[%s][%s]",cKinbr,sMoDiscnt.br_no);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "Z10D" );
        goto ErrExit;
    }
    
    /***设置开户机构和交易机构***/
    set_zd_data("0020" , cKinbr);
    set_zd_data("0030" , cKinbr);
    /*---------------判断贴出贴入行是否同一支付系统清算行-----如果是统一清算行且线上处理的话,报错!!!------------*/
    if(!memcmp(cstlFlg,"SM00",4))/***SM00:线上处理标志,外行!!!***/
    {
        vtcp_log("[%s][%d]--------线上清算：[%s]",__FILE__,__LINE__,cstlFlg);
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_in," or_br_no=%s ",cdscntInAcctSvcr);
        if (ret)
        {
            sprintf(acErrMsg, "[贴入]查询Hv_uniontab表错误!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_out," or_br_no=%s ",caoAcctSvcr);/**TODO 这里需要确认是贴入人开户行还是入账开户行**/
        if (ret)
        {
            sprintf(acErrMsg, "[贴出]查询Hv_uniontab表错误!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        if(!memcmp(sHv_uniontab_in.qs_no,sHv_uniontab_out.qs_no,sizeof(sHv_uniontab_in.qs_no)))
        {
            sMoDiscnt.ac_type[0]='2';/*都走挂账账号**原来的为1 本行账号**/
           /* sprintf(acErrMsg, "错误,线上清算贴出贴入行不能为同一清算行!");
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;*/
        }
	else
	{
        sMoDiscnt.ac_type[0]='2';
	}
    }
    else /******线下清算的情况,就要判断收款人账户是否为我行账户******/
    {
        vtcp_log("[%s][%d] 入账账号为=[%s] ",__FILE__,__LINE__,cpayee_ac_no);
        vtcp_log("[%s][%d]--------线上清算：[%s]",__FILE__,__LINE__,cstlFlg);
        memset(&sHv_uniontab_in,0x00,sizeof(sHv_uniontab_in));
        memset(&sHv_uniontab_out,0x00,sizeof(sHv_uniontab_out));
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_in," or_br_no=%s ",cdscntInAcctSvcr);
        if (ret)
        {
            sprintf(acErrMsg, "[贴入]查询Hv_uniontab表错误!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_out," or_br_no=%s ",caoAcctSvcr);/**TODO 这里需要确认是贴入人开户行还是入账开户行**/
        if (ret)
        {
            sprintf(acErrMsg, "[贴出]查询Hv_uniontab表错误!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        if(!memcmp(sHv_uniontab_in.qs_no,sHv_uniontab_out.qs_no,sizeof(sHv_uniontab_in.qs_no)))
        {
            /*sMoDiscnt.ac_type[0]='2';都走挂账账号**原来的为1 本行账号  20151130 by xmh放开 让其可以记本行账号直接入账**/
            sMoDiscnt.ac_type[0]='1';/*行内需求不走挂账账号**直接记本行账号**/
            
        }
        else 
        {
            sMoDiscnt.ac_type[0]='2';
        }
 
    }
    vtcp_log("[%s][%d] sMoDiscnt.ac_type=[%s] ",__FILE__,__LINE__,sMoDiscnt.ac_type);
    
    
    if(sMoDiscnt.po_knd[0]!='E')
    {
        vtcp_log("[%s][%d] po_knd=[%s] ",__FILE__,__LINE__,sMoDiscnt.po_knd);
        sprintf(acErrMsg,"该笔贴现不是电票系统的ERR!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B103" );
        goto ErrExit;
    }
    vtcp_log("[%s][%d] 报文里传过来的贴出人为=[%s] ac_type=[%s]",__FILE__,__LINE__,cpn_ac_no,sMoDiscnt.ac_type);
    
    /**报文里说如果贴出人为0,则为本行账户,校验本行账户**/
    if(memcmp(cpn_ac_no,"0" ,sizeof(cpn_ac_no)-1)==0)
    {
        vtcp_log("[%s][%d] pn_ac_no=[%s]",__FILE__,__LINE__,sMoDiscnt.pn_ac_no);
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_no ='%s' ",sMoDiscnt.pn_ac_no );
        if (ret) {
            sprintf(acErrMsg, "非我行账户!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B117");
            goto ErrExit;
        }
        ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab_t," or_br_no=%s ",cPayee_brno_name);
        if (ret)
        {
            sprintf(acErrMsg, "[贴入]查询Hv_uniontab表错误!! ret [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        if(memcmp(sHv_uniontab_t.qs_no, HV_QS_BR_NO, 12) != 0)
        {
            sprintf(acErrMsg, "贴现申请人开户行非我黄行户[%s]",sHv_uniontab_t.qs_no);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "D245" );
            goto ErrExit;
        }
    }
    else
    {
        if(memcmp(cpn_ac_no,sMoDiscnt.pn_ac_no,sizeof(sMoDiscnt.pn_ac_no)-1)!=0)
        {
            sprintf(acErrMsg, "与贴现审批数据部一致[%s][%s]",cpn_ac_no,sMoDiscnt.pn_ac_no);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B097" );
            goto ErrExit;
        }
    }
    
    vtcp_log(" [%s][%d] 贴现协议编号:[%s] ",__FILE__,__LINE__,sMoDiscnt.pact_no);
    vtcp_log(" [%s][%d] 出票人帐号:[%s] ",__FILE__,__LINE__,sMoDiscnt.rem_ac_no);
    vtcp_log(" [%s][%d] 收款人帐号:[%s] ",__FILE__,__LINE__,sMoDiscnt.payee_ac_no);
    if(sMoDiscnt.ac_type[0]!='1')
    {
         /**********入账账户非我行账户，先把钱放到本机构应解汇款的账户***********/
         vtcp_log("[%s][%d] br_no =[%s] ",__FILE__,__LINE__,sMoDiscnt.br_no);
         ret = pub_base_GetParm_Str(sMoDiscnt.br_no, PARM_ECDS_TC_AC, cTmpAcno);
         if (ret) {
             sprintf(acErrMsg, "取贴现产品编号错!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "B126");
             goto ErrExit;
         }
    }
    else
    {
         memcpy(cTmpAcno,cpayee_ac_no,sizeof(cTmpAcno)-1);
    }
    vtcp_log("[%s][%d] 入账账户cTmpAcno=[%s] ac_type=[%s]",__FILE__,__LINE__,cTmpAcno,sMoDiscnt.ac_type);
    
    /*-------------------------为spB911准备数据------------------------------*/
    set_zd_data("0690", "1");                      /* 操作码     */
    set_zd_data("0620", sMoDiscnt.pact_no);        /* 贴现协议编号 */        
    set_zd_double("1002", sMoDiscnt.par_amt);      /* 汇票金额   */
    /**long lPnDays=0;
    ret = pub_base_CalDays(sMoDiscnt.pn_date, sMoDiscnt.not_matr_date, "0", &lPnDays);
    if (ret) {
        sprintf(acErrMsg, "计算贴现天数错!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B127");
        goto ErrExit;
    }
    sMoDiscnt.pn_int = sMoDiscnt.par_amt * (lPnDays + sMoDiscnt.way_days) * (sMoDiscnt.pn_rate / 30 / 1000);
    sMoDiscnt.pn_int = pub_base_PubDround_1(sMoDiscnt.pn_int);***/
   if(pub_base_CompDblVal(ddrftAmt,ddrftRAmt)<0)
   {
      sprintf(acErrMsg, "计算贴现天数错!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B127");
        goto ErrExit;
   }
    set_zd_double("0840", sMoDiscnt.pn_rate);      /* 贴现率     */
    set_zd_double("1001",ddrftAmt-ddrftRAmt);      /**贴息**/
    set_zd_long("0450", sEcdsRecvReg.duedt);       /* 汇票到期日 */
    /**本行是贴入行  0310 不应该是出票人账号，应该是人账账号 D099短交易中用到这个域值 tx_sub_rel 映射的**/
    /*set_zd_data("0310", sMoDiscnt.rem_ac_no);       出票人帐号 */
    set_zd_data("0310", sMoDiscnt.pn_ac_no);      /* 人账账号 */
    /**end by xmh 20151101**/
    set_zd_data("0260", sMoDiscnt.remitter);       /* 出票人全称 */
    set_zd_data("0810", sMoDiscnt.payer_brno_name);/* 付款行全称 */
    set_zd_data("0320", sMoDiscnt.payee_ac_no);    /* 收款人帐号 */
    set_zd_data("0270", sMoDiscnt.payee);          /* 收款人全称 */    
    set_zd_data("0820", sMoDiscnt.payee_brno_name);/* 收款人开户行全称 */
    set_zd_data("0900", sMoDiscnt.note_type);      /* 票据类型       */
    set_zd_data("0750", sEcdsRecvReg.drftnb);      /* 票据号         */
    set_zd_data("0670", sMoDiscnt.ac_type);        /* 收款账户类型   */
    set_zd_data("0830", sMoDiscnt.pn_ac_no);       /* 贴现申请人账号 */
    set_zd_data("0250", sMoDiscnt.pn_name);        /* 贴现申请人户名 */
    set_zd_data("0680", sMoDiscnt.local_ind);      /* 异地同城标志 */
    set_zd_long("0480", 0 );                       /* 在途天数     */
    set_zd_data("0980", sMoDiscnt.po_knd);         /* 在途天数     */
    set_zd_data("0630", cTmpAcno);                 /* 贴现协议编号 */ 
    vtcp_log("[%s][%d] 为spB911准备数据[%s]",__FILE__,__LINE__,sMoDiscnt.br_no);
    }
    else if(lw_ind[0]=='2')
    {
		char tx_tel[5];
		memset(tx_tel,0x00,sizeof(tx_tel));
		memcpy(tx_tel,g_pub_tx.tx_br_no+1,2);
		memcpy(tx_tel+2,"99",2);
		strcpy(g_pub_tx.tel,tx_tel);
    	     memset(cTmpAcno, 0  , sizeof(cTmpAcno));  
    	if(memcmp(sHv_uniontab_out.qs_no,sHv_uniontab_in.qs_no,12)==0)
    	{
    		 GetBr_noByOr_br_no(cdscntInAcctSvcr,sMoDiscnt.br_no);
    		 ret = pub_base_GetParm_Str(sMoDiscnt.br_no, PARM_ECDS_TC_AC, cTmpAcno);
         if (ret) {
             sprintf(acErrMsg, "取贴现产品编号错!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "B126");
             goto ErrExit;
         }
         /*******为来账准备数据******/
         set_zd_data("0310",cpayee_ac_no);
         set_zd_data("0300",cTmpAcno);
         set_zd_double("1001",ddrftAmt-ddrftRAmt);      /**贴息**/
         set_zd_double("1002",ddrftAmt);      /**票面金额**/

    	}
    	else 
    	{
    		 GetBr_noByOr_br_no(caoAcctSvcr,sMoDiscnt.br_no);
    		 ret = pub_base_GetParm_Str(sMoDiscnt.br_no, PARM_ECDS_TR_PAY_AC, cTmpAcno);
         if (ret) {
             sprintf(acErrMsg, "取贴现产品编号错!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "B126");
             goto ErrExit;
         }
          /*******为来账准备数据******/
         set_zd_data("0310",cpayee_ac_no);
         set_zd_data("0300",cTmpAcno);
         set_zd_double("1001",ddrftAmt-ddrftRAmt);      /**贴息**/
         set_zd_double("1002",ddrftAmt);      /**票面金额**/
      
    	}
    	         goto OkExit2;

    }
OkExit1:
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;    
OkExit2:
    strcpy(g_pub_tx.reply, "0002");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(!memcmp(g_pub_tx.reply,"0000",4))
    {
    	 strcpy(g_pub_tx.reply, "B554");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
