/*************************************************
* 文 件 名:  spB559.c
            再贴现赎回通知
* 功能描述: 从平台接收数据，准备回购式再贴现业务赎回数据。
* 作    者:  yindianheng
* 完成日期： 2010-4-11
*
* 修改记录：
*   日    期:
*   修 改 人:
*   修改内容:
 
insert into tx_def values ('4959','再贴现赎回通知','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)
insert into tx_flow_def  values ('4959','0','B559','#=0001@1@#$')  
insert into tx_flow_def  values ('4959','1','B945','#=0000@2@#$')  
insert into tx_flow_def  values ('4959','2','A016','#=0000@3@#$')           
insert into tx_flow_def  values ('4959','3','D005','#=0001@4@#=0002@5@#$')           
insert into tx_flow_def  values ('4959','4','L201','#$')           
insert into tx_flow_def  values ('4959','5','a017','#$')
insert into tx_sub_def values ('B559','再贴现赎回通知','spB559','0','0')



insert into  tx_sub_rel (tx_code, in_sub_tx, out_ind,out_data,in_data,memo)
select '4959',in_sub_tx,out_ind,out_data,in_data,memo  from tx_sub_rel
where tx_code ='4945' ;


   

*************************************************/

#include "public.h" 
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "in_mst_c.h"
#include "hv_orno_brno_c.h"
 
#include "com_parm_c.h"
 
 
int spB559()
{ 
    struct ecds_recv_reg_c    sEcds_recv_reg;   
    struct mo_discnt_c    sMo_discnt; 
    struct hv_orno_brno_c    s_Hv_orno_brno;
    struct com_parm_c    sCom_parm;
    struct mdm_ac_rel_c    sMdm_ac_rel;
    struct ecds_recv_reg_c    sEcdsRecvReg1;
    struct in_mst_c		sIn_mst;
    
    /*定义变量*/
    int     ret = 0;
    long    dt           =0    ;              /* 日期                */ 
    long    dueDt        =0 ;                 /* 到期日              */ 
           
    char    stlFlg              [4+1] ;       /* 清算标志            */ 
    char    drftCls             [1+1] ;       /* 票据分类            */ 
    char    drftTyp             [4+1] ;       /* 票据类型            */ 
    char    drftNb              [30+1];       /* 票据号              */ 
    char    tranSeqNb           [28+1];       /* 交易流水号          */ 
    char    intrstTyp           [1+1] ;       /* 利率类型            */ 
    char    origDscntInAcctSvcr [12+1];       /* 原贴入行行号        */ 
    char    origDscntInAcct     [32+1];       /* 原贴入行账号        */ 
    char    origDscntInNm       [60+1];       /* 原贴入行名称        */ 
    char    agtAcctSvcr_1       [12+1];       /* (原贴入+1]承接行行号*/ 
    char    origDscntOutAcctSvcr[12+1];       /* 原贴出行行号        */ 
    char    origDscntOutAcct    [32+1];       /* 原贴出行账号        */ 
    char    origDscntOutNm      [60+1];       /* 原贴出行名称        */ 
    char    agtAcctSvcr_2       [12+1];       /* (原贴出+1]承接行行号*/ 
    
    char    cbuff[61];
    
    double    drftAmt     =0.00 ;             /* 票面金额            */
    double    intrstRate  =0.00 ;             /* 利率                */
    double    amt         =0.00 ;             /* 实付金额            */
   
    /***********初始化结构体和变量***********/
    vtcp_log("[%s][%d] 再贴现赎回通知开始了!!!", __FILE__,__LINE__);
    
    memset(cbuff, 0x00, sizeof(cbuff));
       
    memset(stlFlg                ,0x00,sizeof(stlFlg              ));
    memset(drftCls               ,0x00,sizeof(drftCls             ));
    memset(drftTyp               ,0x00,sizeof(drftTyp             ));
    memset(drftNb                ,0x00,sizeof(drftNb              ));
    memset(intrstTyp             ,0x00,sizeof(intrstTyp           ));
    memset(tranSeqNb             ,0x00,sizeof(tranSeqNb           ));
    memset(origDscntInAcctSvcr   ,0x00,sizeof(origDscntInAcctSvcr ));
    memset(origDscntInAcct       ,0x00,sizeof(origDscntInAcct     ));
    memset(origDscntInNm         ,0x00,sizeof(origDscntInNm       ));
    memset(agtAcctSvcr_1         ,0x00,sizeof(agtAcctSvcr_1       ));
    memset(origDscntOutAcctSvcr  ,0x00,sizeof(origDscntOutAcctSvcr));
    memset(origDscntOutAcct      ,0x00,sizeof(origDscntOutAcct    ));
    memset(origDscntOutNm        ,0x00,sizeof(origDscntOutNm      ));
    memset(agtAcctSvcr_2         ,0x00,sizeof(agtAcctSvcr_2       ));
    
    memset(&s_Hv_orno_brno,0x00,sizeof(struct hv_orno_brno_c));
    memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sMo_discnt,0x00,sizeof(struct mo_discnt_c));
    memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
    memset(&sEcdsRecvReg1, 0x00, sizeof(sEcdsRecvReg1));
    memset(&sIn_mst, 0x00, sizeof(struct in_mst_c));
        
    pub_base_sysinit();
    /**********从平台得到数据**********/
    get_zd_long("0460",&dt );      /* 日期                */        
    
    get_zd_data("0810",cbuff);       /* 交易流水号          */  
    zip_space(cbuff);
    memcpy(tranSeqNb,cbuff,sizeof(tranSeqNb));
    memset(cbuff, 0x00, sizeof(cbuff));
        
    get_zd_data("0580",cbuff );       /* 原贴入行行号        */ 
    zip_space(cbuff);
    memcpy(origDscntInAcctSvcr,cbuff,sizeof(origDscntInAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff));    
    get_zd_data("0820",cbuff     );       /* 原贴入行账号        */ 
    zip_space(cbuff);
    memcpy(origDscntInAcct,cbuff,sizeof(origDscntInAcct));
    memset(cbuff, 0x00, sizeof(cbuff));        
    get_zd_data("0250",origDscntInNm       );       /* 原贴入行名称        */     
    get_zd_data("0590",cbuff      );       /* (原贴入+1]承接行行号*/
    zip_space(cbuff);
    memcpy(agtAcctSvcr_1,cbuff,sizeof(agtAcctSvcr_1));
    memset(cbuff, 0x00, sizeof(cbuff));      
    get_zd_data("0610",cbuff);       /* 原贴出行行号        */
    zip_space(cbuff);
    memcpy(origDscntOutAcctSvcr,cbuff,sizeof(origDscntOutAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff));      
    get_zd_data("0330",cbuff    );       /* 原贴出行账号        */   
    zip_space(cbuff);
    memcpy(origDscntOutAcct,cbuff,sizeof(origDscntOutAcct));
    memset(cbuff, 0x00, sizeof(cbuff));    
    get_zd_data("0260",origDscntOutNm      );       /* 原贴出行名称        */     
    get_zd_data("0600",cbuff       );       /* (原贴出+1]承接行行号*/ 
    zip_space(cbuff);
    memcpy(agtAcctSvcr_2 ,cbuff,sizeof(agtAcctSvcr_2 ));
    memset(cbuff, 0x00, sizeof(cbuff));      
    get_zd_data("0340",stlFlg              );       /* 清算标志            */     
    get_zd_data("0670",drftCls             );       /* 票据分类            */     
    get_zd_data("0910",cbuff             );       /* 票据类型            */
    zip_space(cbuff);
    memcpy(drftTyp ,cbuff,sizeof(drftTyp ));
    memset(cbuff, 0x00, sizeof(cbuff));      
    get_zd_data("0270",cbuff              );       /* 票据号              */ 
    zip_space(cbuff);
    memcpy(drftNb ,cbuff,sizeof(drftNb ));
    memset(cbuff, 0x00, sizeof(cbuff));     
    get_zd_double("0400",&drftAmt          );       /* 票面金额            */        
    get_zd_long("0450",&dueDt              );       /* 到期日              */     
    get_zd_data("0680",intrstTyp           );       /* 利率类型            */     
    get_zd_double("0840",&intrstRate       );       /* 利率                */         
    get_zd_double("0410",&amt              );       /* 实付金额            */        
    
     
    vtcp_log(" [%s][%d] 日期:[%ld]",__FILE__,__LINE__,dt);
    vtcp_log(" [%s][%d] 交易流水号:[%s] ",__FILE__,__LINE__,tranSeqNb);
    vtcp_log(" [%s][%d] 原贴入行行号:[%s] ",__FILE__,__LINE__,origDscntInAcctSvcr);
    vtcp_log(" [%s][%d] 原贴入行账号:[%s] ",__FILE__,__LINE__,origDscntInAcct);
    vtcp_log(" [%s][%d] 原贴入行名称:[%s] ",__FILE__,__LINE__,origDscntInNm);
    vtcp_log(" [%s][%d] 原贴入+1]承接行行号:[%s] ",__FILE__,__LINE__,agtAcctSvcr_1);
    vtcp_log(" [%s][%d] 原贴出行行号:[%s] ",__FILE__,__LINE__,origDscntOutAcctSvcr);
    vtcp_log(" [%s][%d] 原贴出行账号:[%s] ",__FILE__,__LINE__,origDscntOutAcct);
    vtcp_log(" [%s][%d] 原贴出行名称:[%s] ",__FILE__,__LINE__,origDscntOutNm);
    vtcp_log(" [%s][%d] 原贴出+1]承接行行号:[%s] ",__FILE__,__LINE__,agtAcctSvcr_2);
    vtcp_log(" [%s][%d] 清算标志:[%s] ",__FILE__,__LINE__,stlFlg);
                
    vtcp_log(" [%s][%d] 票据类型:[%s] ",__FILE__,__LINE__,drftTyp);
    vtcp_log(" [%s][%d] 票据分类:[%s] ",__FILE__,__LINE__,drftCls);
     
    vtcp_log(" [%s][%d] 票据号码:[%s] ",__FILE__,__LINE__,drftNb);
    vtcp_log(" [%s][%d] 到期日期:[%ld] ",__FILE__,__LINE__,dueDt);
    vtcp_log(" [%s][%d] 票面金额:[%f] ",__FILE__,__LINE__,drftAmt);
    vtcp_log(" [%s][%d] 到期日:[%ld] ",__FILE__,__LINE__,dueDt);
    vtcp_log(" [%s][%d] 利率类型:[%s] ",__FILE__,__LINE__,intrstTyp);
    vtcp_log(" [%s][%d] 利率:    [%f] ",__FILE__,__LINE__,intrstRate);
    vtcp_log(" [%s][%d] 实付金额:    [%f] ",__FILE__,__LINE__,amt);
         
    /*-----------判断该电票是否已经处理过------------*/
    ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
            dt, tranSeqNb);
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
                 tranSeqNb,dt);
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
        vtcp_log(" [%s][%d] ret=[%d] 没有查到相应的信息!!",__FILE__,__LINE__,ret);
        
    /* ------------Ecds_recv_reg数据准备-------------------------*/
         sEcds_recv_reg.ecds_tx_date=dt;
         memcpy(sEcds_recv_reg.ecds_trace_no,tranSeqNb,sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
         sEcds_recv_reg.trace_no = g_pub_tx.trace_no;
         sEcds_recv_reg.tx_date = g_pub_tx.tx_date;        
         memcpy(sEcds_recv_reg.drftcls,drftCls,sizeof(sEcds_recv_reg.drfttyp)-1);     
         memcpy(sEcds_recv_reg.drfttyp ,    drftTyp,sizeof(sEcds_recv_reg.drftnb)-1);     
         memcpy(sEcds_recv_reg.drftnb ,drftNb,    sizeof(sEcds_recv_reg.drftnb)-1);   
         sEcds_recv_reg.issedt = 0 ;    /*???*/
         sEcds_recv_reg.duedt= dueDt ;  
         memcpy(sEcds_recv_reg.tag_name,"4959",sizeof( sEcds_recv_reg.tag_name)-1); 
         sEcds_recv_reg.sts[0]='1';   
         sEcds_recv_reg.bf_flag[0]='0'; 
         memcpy(sEcds_recv_reg.check_flag,"2",sizeof(sEcds_recv_reg.check_flag)-1);/*对账标志*/
         /* ------------Ecds_recv_reg数据准备-------------------------*/
         Ecds_recv_reg_Debug(&sEcds_recv_reg);    
         ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
         if(ret )
         {
             sprintf(acErrMsg, "插入Ecds_recv_reg错!!ret=[%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "0XC8");
             goto ErrExit;
         }
    }
    else
    {
        vtcp_log("查询Ecds_recv_reg表错误!! ret [%d]",ret);
        strcpy ( g_pub_tx.reply , "0T06" );
        goto ErrExit;
    }
     
    
    
    
    /*---------判断转换票据类型-------------------*/
    vtcp_log("[%s][%d]--------票据类型:[%s]",__FILE__,__LINE__,drftTyp);
    if(!memcmp(drftTyp,"AC01",4))
    {
        memcpy(drftTyp,"111",4);
    }
    else if(!memcmp(drftTyp,"AC02",4))
    {
        memcpy(drftTyp,"112",4);        
    }
    else
    {
        sprintf(acErrMsg, "-票据类型错误!!:%s ",drftTyp);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "0T06" );
        goto ErrExit;
        
    }
    /*-sts??---------检查原回购式再贴现的基本信息--------------*/
    ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt,\
        " pnote_no= '%s' and  note_type ='%s' and  pn_type='7' and sts='3' ",\
         drftNb,drftTyp);     
    if (ret)
    {
        sprintf(acErrMsg,"查询mo_discnt表错误!!ret= [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B174" );
        goto ErrExit;
    }
    /*****查找付款账户信息*****/
    ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm,"val_type='5' and  parm_seqn= %d and parm_code='%s' ",PARM_ECDS_TR_PAY_AC,sMo_discnt.br_no);
    if(ret)
    {
        sprintf(acErrMsg, "查询Com_parm表错误!!ret  [%s]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    memset(&sMdm_ac_rel , 0 , sizeof(sMdm_ac_rel));
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no ='%s' ", sCom_parm.val);
    if (ret == 100) {
        sprintf(acErrMsg, "介质账号对照表中不存在此账号!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;
    } else if (ret) {
        sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret);
        WRITEMSG
         strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;
    }
    vtcp_log(" [%s][%d]  sMdm_ac_rel.note_sts:[%s] ",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
    /*****增加对内部帐的检查,账户状态及余额  TODO *****/
    if(sMdm_ac_rel.note_sts[0]!='0')
    {
    	sprintf(acErrMsg, " 介质状态不正常!sMdm_ac_rel.note_sts=[%s]", sMdm_ac_rel.note_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;
    	
    }
    vtcp_log(" [%s][%d]  sMdm_ac_rel.note_sts:[%s] ",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
   
    ret =In_mst_Sel(g_pub_tx.reply, &sIn_mst, "ac_id ='%ld' and ac_seqn='%d' ", sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
    if (ret == 100) {
        sprintf(acErrMsg, "In_mst中不存在此ac_id:%ld!",sMdm_ac_rel.ac_id);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B124");
        goto ErrExit;
    } else if (ret) {
        sprintf(acErrMsg, "执行In_mst_Sel错![%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B124");
        goto ErrExit;
    }
    if(sIn_mst.sts[0]!='1')
    {
    	sprintf(acErrMsg, " 内部帐号状态不正常!sLn_mst.sts=[%s]", sIn_mst.sts);
        WRITEMSG
        strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;    	
    }
    vtcp_log("[%s][%d]--- amt[%f].sLn_mst.bal=[%f]", __FILE__,__LINE__,amt,sIn_mst.bal);
    if(amt>sIn_mst.bal)
    {
    	sprintf(acErrMsg, " 金额不足! 错");
        WRITEMSG
        strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;   
    	
    }
    
    
    vtcp_log("[%s][%d]--- sCom_parm.val=%s", __FILE__,__LINE__,sCom_parm.val);
    vtcp_log("[%s][%d]--- 再贴现协议编号=%s", __FILE__,__LINE__,sMo_discnt.rpact_no);
        
    vtcp_log("[%s][%d] rpact_no=[%s] val=[%s]", __FILE__,__LINE__,sMo_discnt.rpact_no,sCom_parm.val);
    set_zd_data("0330",sMdm_ac_rel.ac_no);
    set_zd_data("0620", sMo_discnt.rpact_no);    /* 再贴现协议编号 */
    set_zd_data("0300", sCom_parm.val);          /* 人行准备金账号 */
    set_zd_data("0720", "2");                    /* 央行收款 1: 已收  2: 未收*/
    set_zd_double("0840",intrstRate);            /* 再贴现率 */
    vtcp_log("[%s][%d]----B945数据准备完 ", __FILE__,__LINE__);
    /* ---------------------------------------*/
GoodExit1:
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;    
GoodExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(!memcmp(g_pub_tx.reply,"0000",4))
    {
    	 strcpy(g_pub_tx.reply, "B559");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
