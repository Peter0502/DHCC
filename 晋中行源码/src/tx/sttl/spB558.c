/************************************************************************************
* 文 件 名:  spB558.c
* 功能描述： 再贴现通知
*            从平台得到基本信息，为再贴现的录入记账准备数据.
* 作    者: yindianheng
* 完成日期： 2010年4月10日
*
* 修改记录：
*   日   期:
*   修 改 人: xyy
*   修改内容: 2010-4-27 22:45:44 

insert into tx_def values ('4958','再贴现通知','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)

insert into tx_sub_def values ('B558','再贴现通知','spB558','0','0')

insert into tx_flow_def   values ('4958','0','B558','#=0001@1@#=0002@5@#$')
insert into tx_flow_def   values ('4958','1','B943','#=0000@2@#$')
insert into tx_flow_def   values ('4958','2','B944','#=0000@4@#$')
----insert into tx_flow_def   values ('4958','3','A017','#=0000@4@#$')
insert into tx_flow_def   values ('4958','4','A016','#$')

insert into tx_flow_def   values ('4958','5','B943','#=0000@6@#$')
insert into tx_flow_def   values ('4958', 6, 'L201', '#=0000@7@#$');
insert into tx_flow_def   values ('4958','7','B944','#=0000@8@#$');
insert into tx_flow_def   values ('4958', 8, 'a016', '#=0000@9@#$');
insert into tx_flow_def   values ('4958', 9, 'L320', '#=0001@10@#$');
insert into tx_flow_def   values ('4958', 10, 'b016', '#=0000@11@#$');
insert into tx_flow_def   values ('4958', 11, 'a017', '#$');

insert into tx_sub_rel  values ('4958', 'a016', '0', '112A', '1201', '帐号');
insert into tx_sub_rel  values ('4958', 'a016', '1', '01', '1204', '币种');
insert into tx_sub_rel  values ('4958', 'a016', '1', '2', '1205', '转账');
insert into tx_sub_rel  values ('4958', 'a016', '0', '1002-1001', '1208', '发生额');

insert into tx_sub_rel  values ('4958', 'L320', '0', '0630', '0620', null);

insert into tx_sub_rel  values ('4958', 'L201', '1', '01', '0210', null);
insert into tx_sub_rel  values ('4958', 'L201', '1', '9', '0660', null);
insert into tx_sub_rel  values ('4958', 'L201', '0', '0330', '1071', null);
insert into tx_sub_rel  values ('4958', 'L201', '1', '0', '1072', null);
insert into tx_sub_rel  values ('4958', 'L201', '1', '1', '107A', null);
insert into tx_sub_rel  values ('4958', 'L201', '0', '1002', '107B', null);


insert into tx_sub_rel values ('4958','B944','0','0700','0670','')

insert into tx_sub_rel values ('4958','A016','0','0300','1201','帐号')
insert into tx_sub_rel values ('4958','A016','1','01','1204','币种')
insert into tx_sub_rel values ('4958','A016','1','2','1205','现转')
insert into tx_sub_rel values ('4958','A016','0','1003','1208','金额')
insert into tx_sub_rel values ('4958','A016','1','电票再贴现借记账','120A','附言')

----insert into tx_sub_rel values ('4958','A017','1','202201','1211','帐户')
----insert into tx_sub_rel values ('4958','A017','1','2','1215','现转')
----insert into tx_sub_rel values ('4958','A017','0','1002','1218','金额')
----insert into tx_sub_rel values ('4958','A017','1','电票再贴现贷记账','121A','附言')
**************************************************************************************************/
#define EXTERN
#include "public.h" 
#include <string.h>
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "ln_mst_c.h"
#include "hv_orno_brno_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"

int 
spB558()
{ 
    /******定义平台传来的变量******/
    
    int         ret = 0;
    long        dt=0;                   /*日期  */
    long        dueDt=0;                /*到期日*/
    
    char        cKinbr[BRNO_LEN+1];
    char        stlFlg[4+1];            /*清算标志*/
    char        drftCls[1+1];           /*票据分类*/
    char        drftTyp[4+1];           /*票据类型*/
    char        drftNb[30+1];           /*票 据 号*/
    char        dscntTyp[4+1];          /*贴现类型*/
    char        intrstTyp[1+1];         /*利率类型*/
    char        cRpact_no[20+1];        /*再贴现号码*/
    char        tranSeqNb[28+1];        /*交易流水号*/
    
    char        dscntInNm[60+1];        /*贴入行名称*/
    char        dscntOutNm[60+1];       /*贴出行名称*/
    char        dscntOutAcct[32+1];     /*贴出行账号*/
    char        dscntInAcct[32+1];      /*贴入行账号*/
    char        dscntOutAcctSvcr[12+1]; /*贴出行行号*/
    char        dscntInAcctSvcr[12+1];  /*贴入行行号*/
    char        agtAcctSvcr_1[12+1];    /*(贴出)承接行行号*/
    
    char        cbuff[61];
    double      intrstRate=0;         /*利率*/
    double      amt=0.00;             /*实付金额*/
    double      drftAmt=0.00;         /*票面金额*/
    
    struct com_parm_c         sCom_parm;
    struct mo_discnt_c        sMo_discnt;
    struct mdm_ac_rel_c       sMdm_ac_rel;
    struct ecds_recv_reg_c    sEcds_recv_reg;   
    struct ecds_recv_reg_c    sEcdsRecvReg1;
    
    vtcp_log("[%s][%d] define  Variable!!!", __FILE__,__LINE__); 
     
    /*******初始化变量和结构体******/
    memset(&sCom_parm     , 0 , sizeof(struct com_parm_c));
    memset(&sMo_discnt    , 0 , sizeof(struct mo_discnt_c));
    memset(&sMdm_ac_rel   , 0 , sizeof(struct mdm_ac_rel_c));
    memset(&sEcdsRecvReg1 , 0 , sizeof(struct ecds_recv_reg_c));
    memset(&sEcds_recv_reg, 0 , sizeof(struct ecds_recv_reg_c));
    
    memset ( cKinbr             , 0  ,sizeof( cKinbr           ));
    memset ( tranSeqNb          ,0x00,sizeof( tranSeqNb        )); 
    memset ( dscntTyp           ,0x00,sizeof( dscntTyp         )); 
    memset ( dscntInAcctSvcr    ,0x00,sizeof( dscntInAcctSvcr  )); 
    memset ( dscntInAcct        ,0x00,sizeof( dscntInAcct      )); 
    memset ( dscntInNm          ,0x00,sizeof( dscntInNm        )); 
    memset ( dscntOutAcctSvcr   ,0x00,sizeof( dscntOutAcctSvcr )); 
    memset ( dscntOutAcct       ,0x00,sizeof( dscntOutAcct     )); 
    memset ( dscntOutNm         ,0x00,sizeof( dscntOutNm       )); 
    memset ( agtAcctSvcr_1      ,0x00,sizeof( agtAcctSvcr_1    )); 
    memset ( stlFlg             ,0x00,sizeof( stlFlg           )); 
    memset ( drftCls            ,0x00,sizeof( drftCls          )); 
    memset ( drftTyp            ,0x00,sizeof( drftTyp          )); 
    memset ( drftNb             ,0x00,sizeof( drftNb           )); 
    memset ( intrstTyp          ,0x00,sizeof( intrstTyp        )); 
    memset ( cRpact_no		,0X00,sizeof( cRpact_no	       ));
    memset(cbuff, 0x00, sizeof(cbuff)); 
    
    pub_base_sysinit();    
    vtcp_log("[%s][%d] 电票业务的再贴现通知来了!",__FILE__,__LINE__);
    
    
  
    /**********************接收的报文内容*************************/
    get_zd_data ( "0810",      cbuff       );     /*交易流水号*/   
    zip_space(cbuff);
    memcpy(tranSeqNb,cbuff,sizeof(tranSeqNb));
    memset(cbuff, 0x00, sizeof(cbuff));        
               
    get_zd_data ( "0580",      cbuff );     /*贴入行行号*/ 
    zip_space(cbuff);
    memcpy(dscntInAcctSvcr,cbuff,sizeof(dscntInAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff));              
    get_zd_data ( "0820",      cbuff     );     /*贴入行账号*/
    zip_space(cbuff);
    memcpy(dscntInAcct,cbuff,sizeof(dscntInAcct));
    memset(cbuff, 0x00, sizeof(cbuff));               
    
    vtcp_log("[%s][%d] tranSeqNb=[%s] ",__FILE__,__LINE__,tranSeqNb);
    vtcp_log("[%s][%d] dscntInAcctSvcr=[%s] dscntInAcct=[%s]",__FILE__,__LINE__,dscntInAcctSvcr,dscntInAcct);
    
    get_zd_data ( "0250",      dscntInNm       );     /*贴入行名称*/           
    get_zd_data ( "0590",      cbuff);     /*贴出行行号*/ 
    zip_space(cbuff);
    memcpy(dscntOutAcctSvcr,cbuff,sizeof(dscntOutAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff));              
    get_zd_data ( "0330",      cbuff    );     /*贴出行账号*/
    zip_space(cbuff);
    memcpy(dscntOutAcct,cbuff,sizeof(dscntOutAcct));
    memset(cbuff, 0x00, sizeof(cbuff));               
    get_zd_data ( "0260",      dscntOutNm      );     /*贴出行名称*/
    
    vtcp_log("[%s][%d] dscntInNm=[%s] dscntOutAcctSvcr=[%s]",__FILE__,__LINE__,dscntInNm,dscntOutAcctSvcr);
    vtcp_log("[%s][%d] dscntOutAcct=[%s] dscntOutNm=[%s]",__FILE__,__LINE__,dscntOutAcct,dscntOutNm);
    
    
    get_zd_data ( "0600",      cbuff   );     /*(贴出)承接行行号*/
    zip_space(cbuff);
    memcpy(agtAcctSvcr_1,cbuff,sizeof(agtAcctSvcr_1));
    memset(cbuff, 0x00, sizeof(cbuff));       
    get_zd_data ( "0340",      stlFlg          );     /*清算标志*/             
    get_zd_data ( "0670",      drftCls         );     /*票据分类*/             
    get_zd_data ( "0910",      cbuff         );     /*票据类型*/
    zip_space(cbuff);
    memcpy(drftTyp,cbuff,sizeof(drftTyp));
    memset(cbuff, 0x00, sizeof(cbuff));         
    
    vtcp_log("[%s][%d] agtAcctSvcr_1=[%s] stlFlg=[%s]",__FILE__,__LINE__,agtAcctSvcr_1,stlFlg);
    vtcp_log("[%s][%d] drftCls=[%s] drftTyp=[%s]",__FILE__,__LINE__,drftCls,drftTyp);   
    
    get_zd_data ( "0680",      intrstTyp       );     /*利率类型*/             
    get_zd_data ( "0270",      drftNb          );     /*票据号*/
    
    vtcp_log("[%s][%d] intrstTyp=[%s] drftNb=[%s]",__FILE__,__LINE__,intrstTyp,drftNb);
    get_zd_data ( "0640",      cbuff        );     /*贴现类型*/ 
    zip_space(cbuff);
    memcpy(dscntTyp,cbuff,sizeof(dscntTyp));
    memset(cbuff, 0x00, sizeof(cbuff));  
    vtcp_log("[%s][%d] 贴现类型 dscntTyp=[%s]",__FILE__,__LINE__,dscntTyp);
    
    get_zd_long ( "0460",      &dt             );     /*日期*/
    get_zd_long ( "0450",      &dueDt          );     /*到期日*/
    get_zd_double ( "0400",    &drftAmt        );     /*票面金额*/                    
    get_zd_double ( "0410",    &amt            );     /*实付金额*/
    get_zd_double ( "0840",    &intrstRate     );     /*利率*/ 
    vtcp_log("[%s][%d] 利率 intrstRate=[%f]",__FILE__,__LINE__,intrstRate);
    
    vtcp_log("[%s][%d] dt=[%ld] dueDt=[%ld]",__FILE__,__LINE__,dt,dueDt);
    vtcp_log("[%s][%d] drftAmt=[%f] amt=[%f]",__FILE__,__LINE__,drftAmt,amt);
    vtcp_log("[%s][%d] 贴现类型 dscntTyp=[%s]",__FILE__,__LINE__,dscntTyp);
    vtcp_log("[%s][%d] 利率 intrstRate=[%f]",__FILE__,__LINE__,intrstRate);
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
        vtcp_log("[%s][%d] ret=[%d] Ecds_recv_reg没有查到信息,insert",__FILE__,__LINE__,ret);
         
      /*-----------Ecds_recv_reg_Ins------------*/
        sEcds_recv_reg.ecds_tx_date     = dt;                    /**交易日期**/
        sEcds_recv_reg.trace_no         = g_pub_tx.trace_no;     /**核心流水**/
        sEcds_recv_reg.tx_date          = g_pub_tx.tx_date;      /**核心日期**/
        sEcds_recv_reg.issedt           = 0  ;                   /**出票日,不用关心**/
        sEcds_recv_reg.duedt            = dueDt ;                /**汇票到期日**/
        
        sEcds_recv_reg.sts[0]='1';   /**1-正常接收记账  2-交易账务撤销  **/
        sEcds_recv_reg.bf_flag[0]='0';
        memcpy(sEcds_recv_reg.drftcls ,    drftCls,sizeof(sEcds_recv_reg.drftcls)-1);  /**E-电票 F-纸票 **/   
        memcpy(sEcds_recv_reg.drfttyp ,    drftTyp,sizeof(sEcds_recv_reg.drfttyp)-1);   /**AC01-银承 AC01-商承**/
        memcpy(sEcds_recv_reg.drftnb  ,    drftNb ,sizeof(sEcds_recv_reg.drftnb)-1);   /*票据号*/
        memcpy(sEcds_recv_reg.tag_name,    "CBS1000100",sizeof( sEcds_recv_reg.tag_name)-1); /**再贴现通知**/
        memcpy(sEcds_recv_reg.check_flag,"2",sizeof(sEcds_recv_reg.check_flag)-1);     /*对账标志*/
        memcpy(sEcds_recv_reg.ecds_trace_no,tranSeqNb , sizeof(sEcds_recv_reg.ecds_trace_no)-1);/*交易流水号*/ 
        
        Ecds_recv_reg_Debug(&sEcds_recv_reg);
        ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
        if(ret )
        {
            sprintf(acErrMsg, "插入Ecds_recv_reg错!!ret=[%d]", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "B174");
            goto ErrExit;
        }
    }
    else
    {
        sprintf(acErrMsg,"查询Ecds_recv_reg表错误!! ret [%s]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }   
   
    /*---------判断转换票据类型-------------------*/
    vtcp_log("[%s][%d]--------票据类型:[%s]",__FILE__,__LINE__,sEcds_recv_reg.drfttyp);
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
        sprintf(acErrMsg, "---票据类型错误!! ");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
        
    }
    /*--------------------查询原贴现票据的基本信息-----------------------*/
    ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt," pnote_no= '%s' and  note_type ='%s' and sts='3' \
        and pn_type in ('1','2','3') ", drftNb,drftTyp);
    if (ret)
    {
        sprintf(acErrMsg,"查询mo_discnt表错误!!ret= [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B112" );
        goto ErrExit;
    }
    /**************校验平台传过来的数据*****************/
      
     vtcp_log("[%s][%d]  数据库:[%f]平台:[%f]",__FILE__,__LINE__,sMo_discnt.par_amt,drftAmt);
     
    if(pub_base_CompDblVal(drftAmt,sMo_discnt.par_amt) != 0)
    {
        vtcp_log("[%s][%d] sMo_discnt.par_amt=[%f] drftAmt =[%f]",__FILE__,__LINE__,sMo_discnt.par_amt,drftAmt);
        sprintf(acErrMsg,"检查平台送来的票面金额错误!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B103" );
        goto ErrExit;
    } 
    vtcp_log("[%s][%d]  数据库--:[%f]平台:[%f]",__FILE__,__LINE__,sMo_discnt.par_amt,drftAmt);
     
    if(dueDt != sMo_discnt.not_matr_date)
    {
        vtcp_log("[%s][%d] sMo_discnt.not_matr_date=[%ld] duedt =[%ld]",__FILE__,__LINE__,sMo_discnt.not_matr_date,dueDt);
        sprintf(acErrMsg,"检查平台送来的汇票到期日错误!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B094" );
        goto ErrExit;
    }
    
    GetBr_noByOr_br_no(dscntOutAcctSvcr,cKinbr);
    vtcp_log("[%s][%d] cKinbr=[%s][%s] ",__FILE__,__LINE__,cKinbr,sMo_discnt.br_no);

    if(memcmp(cKinbr,sMo_discnt.br_no,BRNO_LEN)!=0)
    {
        sprintf(acErrMsg,"非我行票据=[%s][%s]",cKinbr,sMo_discnt.br_no);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "Z10D" );
        goto ErrExit;
    }
    
    /***设置开户机构和交易机构***/
    set_zd_data("0020" , cKinbr);
    set_zd_data("0030" , cKinbr);
    
    vtcp_log("[%s][%d] 注意两个到期日:[%ld][%ld]",__FILE__,__LINE__,sMo_discnt.not_matr_date,dueDt);
    
  
    vtcp_log("[%s][%d] 为再贴现的录入和记账做好准备![%s]",__FILE__,__LINE__,dscntTyp);
    set_zd_data("0640",dscntTyp);/**RM00:买断式再贴现  RM01:回购式再贴现**/
    
    /********************如何编造一个再贴现协议编号************************/
    char cTmpBuff[9];
    char cTraceNo[10];
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    sprintf(cTmpBuff ,"%08d" , g_pub_tx.tx_date);
    vtcp_log("[%s][%d] cTmpBuff=[%s] ",__FILE__,__LINE__,cTmpBuff);
    
    sprintf(cRpact_no ,"%08d" , g_pub_tx.tx_date);
    memcpy(cRpact_no , cTmpBuff+2 , 6);
    vtcp_log("[%s][%d] cRpact_no=[%s] ",__FILE__,__LINE__,cRpact_no);
    strcat(cRpact_no , sMo_discnt.br_no );
    vtcp_log("[%s][%d] cRpact_no=[%s] trace_no=[%ld]",__FILE__,__LINE__,cRpact_no,g_pub_tx.trace_no);
    
    sprintf(cTraceNo,"%09d",g_pub_tx.trace_no);
    strcat(cRpact_no , cTraceNo );
    vtcp_log("[%s][%d]再贴现协议编号=[%s]", __FILE__,__LINE__,cRpact_no);

    /******************TODO 这里要考虑一下是哪个机构来干这个活*****************/
    ret=Com_parm_Sel(g_pub_tx.reply,&sCom_parm, "parm_code ='%s' and parm_seqn = %d ",sMo_discnt.br_no,PARM_ECDS_TR_PAY_AC);
    if(ret){
         sprintf(acErrMsg,"查询公共参数错误![%d]",__FILE__,__LINE__,ret);
         WRITEMSG
         strcpy(g_pub_tx.reply,"D101");
         goto ErrExit;
    }
    vtcp_log("[%s][%d] 再贴现的记账账号=[%s]",__FILE__,__LINE__,sCom_parm.val);

    set_zd_data("0670", "0");                       /*  sp943.c 为操作类型：0 录入 1 修改 */

    set_zd_data("0620", cRpact_no);                  /* 再贴现协议编号 */
    set_zd_data("0700","1");                         /* sp944.c里表示收款账户类型----tx_sub_rel重新配置1  TODO   */
    set_zd_double("1002", drftAmt);                  /* 汇票金额   */
    set_zd_double("1001", drftAmt - amt);            /* 再贴现利息=汇票金额-实付金额s */
    set_zd_double("0840", intrstRate);               /* 再贴现率   */
    set_zd_data("0250", dscntInNm);                  /* 贴入行名称 */
    
    set_zd_data( DC_OPN_BR_NO , sMo_discnt.br_no);/*开户机构号码为Mo_discnt机构号*/   
    /***********对于再贴现而言,回购日期不定***********/
    set_zd_long("0480",  0 );                       /* 电票在途天数为0 */
    set_zd_long("0460",  0 );                       /* 回购日期 */     
    
    /*****************************以下部分用原票据信息***********************************/
    set_zd_long("0450", dueDt);                      /* 汇票到期日 */
    set_zd_data("0310", sMo_discnt.rem_ac_no);       /* 出票人帐号 */
    set_zd_data("0260", sMo_discnt.remitter);        /* 出票人全称 */
    
    set_zd_data("0810", sMo_discnt.payer_brno_name); /* 付款行全称 */
    set_zd_data("0320", sMo_discnt.payee_ac_no);     /* 收款人帐号 */
    set_zd_data("0270", sMo_discnt.payee);           /* 收款人全称 */
    set_zd_data("0820", sMo_discnt.payee_brno_name); /* 收款人开户行全称 */
    set_zd_data("0900", drftTyp);                    /* 票据类型 */
    set_zd_data("0750", sMo_discnt.pnote_no);        /* 票据号   */
    
    /************************若为买断式再贴现要销记原贴现资产*************************/

    vtcp_log("[%s][%d] dscntTyp=[%s]",__FILE__,__LINE__,dscntTyp);
    if(memcmp(dscntTyp,"RM01",4)==0)/**回购式**/
    {
        /***********为A016准备数据***********/
        set_zd_data("0300", sCom_parm.val);              /* 借方账户 */
        vtcp_log("[%s][%d] 借方账户=[%s]", __FILE__,__LINE__,sCom_parm.val);
        strcpy ( g_pub_tx.reply , "0001" );
    }
    else if (memcmp(dscntTyp,"RM00",4)==0)/**买断式**/
    {
        vtcp_log("[%s][%d] 贷款ac_id=[%ld]",__FILE__,__LINE__,sMo_discnt.ac_id);
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_id=%ld ", sMo_discnt.ac_id);
        if (ret) {
           sprintf(acErrMsg, "查询mdm_ac_rel出错![%s]", sMo_discnt.ac_id);
           WRITEMSG
           strcpy(g_pub_tx.reply, "L015");
           goto ErrExit;
        }
        /***********L320**********/
        set_zd_data("0630" , sMo_discnt.pact_no);
        /***********a016**********/
        set_zd_data("112A", sCom_parm.val);              /* 借方账户 */
        vtcp_log("[%s][%d] 借方账户=[%s]", __FILE__,__LINE__,sCom_parm.val);
        set_zd_double("100E", drftAmt);                  /* 汇票金额   */
        /************L201***********/
        set_zd_data("0330",sMdm_ac_rel.ac_no);
        vtcp_log("[%s][%d] 贷款账户=[%s]",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
        strcpy ( g_pub_tx.reply , "0002" ); 
    }
    else
    {
        sprintf(acErrMsg,"错误,再贴现类型错误!! [%s] ",dscntTyp);
        WRITEMSG 
        goto ErrExit; 
    }
     
    vtcp_log("[%s][%d] 从平台接收数据完成[%s]!!",__FILE__,__LINE__,sMo_discnt.br_no);

OkExit:
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(!memcmp(g_pub_tx.reply,"0000",4))
    {
    	 strcpy(g_pub_tx.reply, "B558");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
