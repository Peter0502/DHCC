/*************************************************
* 文 件 名:  spB556.c
*    转贴现通知
*    作为贴出行，收到转贴现通知后，要处理原贴现资产，新增贴现负债。
*    做为贴入行，收到转贴现通知后，要新增贴现资产，增加转贴现登记，
*    其中转入是由信贷系统发起，核心系统审核后更改相关账务信息及状态。
* 作    者:  chenchao
* 完成日期： 2010-4-9 20:30:43
*
*  
* 修改记录：
*   日    期:
*   修 改 人: xyy
*   修改内容:
*   注    意： 农信银中心的同志说，这个报文只处理线上清算的，那么就说明不能处理同清算中心的业务。xyy
insert into tx_def values ('4956','转贴现通知','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)
insert into tx_flow_def  values ('4956',0,'B556','#=0001@1@#=0002@14@#=0003@15@#$');
insert into tx_flow_def  values ('4956',1,'B921','#=0000@2@#$');
insert into tx_flow_def  values ('4956',2,'B922','#=0000@5@#$');

insert into tx_flow_def  values ('4956', 3, 'L201', '#=0000@7@#$');
insert into tx_flow_def  values ('4956', 4, 'B947', '#=0000@12@#$');

insert into tx_flow_def  values ('4956', 5, 'L104', '#=0002@6@#$');
insert into tx_flow_def  values ('4956', 6, 'a017', '#$');

insert into tx_flow_def  values ('4956', 7,  'B932', '#=0000@8@#$');
insert into tx_flow_def  values ('4956', 8,  'a016', '#=0000@9@#$');
insert into tx_flow_def  values ('4956', 9,  'L320', '#=0001@10@#$');
insert into tx_flow_def  values ('4956', 10, 'A016', '#=0000@11@#$');
insert into tx_flow_def  values ('4956', 11, 'A017', '#$');

insert into tx_flow_def  values ('4956', 12, 'b017', '#=0000@13@#$');
insert into tx_flow_def  values ('4956', 13, 'b016', '#$');

insert into tx_flow_def  values ('4956', 14, 'B931', '#=0000@3@#$');
insert into tx_flow_def  values ('4956', 15, 'B931', '#=0000@4@#$');


insert into tx_sub_def values ('B556','转贴现通知','spB556','0','0')

insert into tx_sub_rel  values ('4956', 'L104', '1', '0', '0340', null);
insert into tx_sub_rel  values ('4956', 'L104', '0', '0330', '0370', null);
insert into tx_sub_rel  values ('4956', 'L104', '0', '1002', '0390', null);
insert into tx_sub_rel  values ('4956', 'L104', '1', '2', '0660', null);

insert into tx_sub_rel  values ('4956', 'a016', '0', '112A', '1201', '帐号');
insert into tx_sub_rel  values ('4956', 'a016', '1', '01', '1204', '币种');
insert into tx_sub_rel  values ('4956', 'a016', '1', '2', '1205', '转账');
insert into tx_sub_rel  values ('4956', 'a016', '0', '1002-1001', '1208', '发生额');

insert into tx_sub_rel  values ('4956', 'a017', '0', '0300', '1211', '同城提出');
insert into tx_sub_rel  values ('4956', 'a017', '1', '01', '1214', '币种');
insert into tx_sub_rel  values ('4956', 'a017', '1', '2', '1215', '转账');
insert into tx_sub_rel  values ('4956', 'a017', '0', '0890', '1216', '凭证种类');
insert into tx_sub_rel  values ('4956', 'a017', '0', '0580', '1217', '凭证号码');
insert into tx_sub_rel  values ('4956', 'a017', '0', '1002-1001', '1218', '金额');
insert into tx_sub_rel  values ('4956', 'a017', '1', '同城提出', '121A', '摘要');

insert into tx_sub_rel  values ('4956', 'L201', '1', '01', '0210', null);
insert into tx_sub_rel  values ('4956', 'L201', '1', '9', '0660', null);
insert into tx_sub_rel  values ('4956', 'L201', '0', '0330', '1071', null);
insert into tx_sub_rel  values ('4956', 'L201', '1', '0', '1072', null);
insert into tx_sub_rel  values ('4956', 'L201', '1', '1', '107A', null);
insert into tx_sub_rel  values ('4956', 'L201', '0', '1002', '107B', null);

insert into tx_sub_rel  values ('4956', 'b016', '0', '0300', '1201', '帐号');
insert into tx_sub_rel  values ('4956', 'b016', '1', '01', '1204', '币种');
insert into tx_sub_rel  values ('4956', 'b016', '1', '2', '1205', '现转');
insert into tx_sub_rel  values ('4956', 'b016', '0', '1003', '1208', '金额');
insert into tx_sub_rel  values ('4956', 'b016', '1', '有追索权转贴现记账', '120A', '附言');

insert into tx_sub_rel  values ('4956', 'b017', '1', '202101', '1211', '帐户');
insert into tx_sub_rel  values ('4956', 'b017', '1', '2', '1215', '现转');
insert into tx_sub_rel  values ('4956', 'b017', '0', '1002', '1218', '金额');
insert into tx_sub_rel  values ('4956', 'b017', '1', '有追索权转贴现记账', '121A', '附言');
*************************************************/
#define ERR_DEAL if( ret ) \
{\
    sprintf( acErrMsg, "sql error" ); \
        WRITEMSG \
        goto ErrExit; \
}
#define EXTERN
#include "public.h" 
#include "hv_define.h"
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"
#include "hv_uniontab_c.h"

int 
spB556() 
{ 
    struct ecds_recv_reg_c    sEcds_recv_reg,s_Ecds_recv_reg;   
    struct mo_discnt_c    sMo_discnt; 
    struct com_parm_c    sCom_parm;
    struct mdm_ac_rel_c   sMdm_ac_rel;
    struct hv_uniontab_c     sHv_uniontab1 , sHv_uniontab2;

    /******************定义变量和机构体***************/
    int        ttlnum = 0;            /**读取的总条数**/
    int        cnt=0;
    int        i = 0;
    int        ret = 0;
    
    long        dt=0;                /*日期*/
    long        dueDt=0;             /*到期日*/
    long        l_date=0;
    char        cTmpBuff[101];
    char        cTpact_no[20+1];        /*转贴现号码*/
    char        cKinbr[BRNO_LEN+1];      
    char        stlFlg[4+1];             /*清算标志*/
    char        drftCls[1+1];            /*票据分类*/
    char        drftTyp[4+1];            /*票据类型*/
    char        drftNb[30+1];            /*票据号  */
    char        drwrAcct[32+1];          /*出票人账号*/
    char        drwrNm[60+1];            /*出票人名称*/  
    char        tranSeqNb[28+1];         /*交易流水号*/
    char        intrstTyp[1+1];          /*利率类型*/
    char        dscntTyp[4+1];           /*贴现类型*/
    char        dscntInNm[60+1];         /*贴入行名称*/
    char        dscntOutNm[60+1];        /*贴出行名称*/
    char        dscntInAcctSvcr[12+1];   /*贴入行行号*/
    char        dscntInAcct[32+1];       /*贴入行账号*/
    char        dscntOutAcctSvcr[12+1];  /*贴出行行号*/
    char        dscntOutAcct[32+1];      /*贴出行账号*/
    char        agtAcctSvcr_1[12+1];     /*(贴出)承接行行号*/
    char    lw_ind[2]; 
    double      drftAmt=0.00;            /*票面金额*/
    double      intrstRate=0.00;         /*利率    */
    double      amt=0.00;                /*实付金额*/
    
    vtcp_log("[%s][%d]", __FILE__,__LINE__);  
    
    memset(&sMo_discnt    , 0 , sizeof(struct mo_discnt_c));
    memset(&sCom_parm     , 0 , sizeof(struct com_parm_c));
    memset(&sMdm_ac_rel   , 0 , sizeof(struct mdm_ac_rel_c));
    memset(&sHv_uniontab1 , 0 , sizeof(struct hv_uniontab_c));
    memset(&sHv_uniontab2 , 0 , sizeof(struct hv_uniontab_c));
    memset(&sEcds_recv_reg, 0 , sizeof(struct ecds_recv_reg_c));
    memset(&s_Ecds_recv_reg, 0 , sizeof(struct ecds_recv_reg_c));
    memset(lw_ind,0x00,sizeof(lw_ind));
    memset ( cKinbr             ,0 ,sizeof(cKinbr            ));
    memset ( stlFlg             ,0 ,sizeof( stlFlg           )); 
    memset ( drftCls            ,0 ,sizeof( drftCls          )); 
    memset ( drftTyp            ,0 ,sizeof( drftTyp          )); 
    memset ( drftNb             ,0 ,sizeof( drftNb           )); 
    memset ( drwrNm             ,0 ,sizeof( drwrNm           ));
    memset ( intrstTyp          ,0 ,sizeof( intrstTyp        )); 
    memset ( cTpact_no          ,0 ,sizeof( cTpact_no        ));
    memset ( drwrAcct           ,0 ,sizeof( drwrAcct         )); 
    memset ( tranSeqNb          ,0 ,sizeof( tranSeqNb        )); 
    memset ( dscntTyp           ,0 ,sizeof( dscntTyp         )); 
    memset ( dscntInAcctSvcr    ,0 ,sizeof( dscntInAcctSvcr  )); 
    memset ( dscntInAcct        ,0 ,sizeof( dscntInAcct      )); 
    memset ( dscntInNm          ,0 ,sizeof( dscntInNm        )); 
    memset ( dscntOutAcctSvcr   ,0 ,sizeof( dscntOutAcctSvcr )); 
    memset ( dscntOutAcct       ,0 ,sizeof( dscntOutAcct     )); 
    memset ( dscntOutNm         ,0 ,sizeof( dscntOutNm       )); 
    memset ( agtAcctSvcr_1      ,0 ,sizeof( agtAcctSvcr_1    )); 

    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    pub_base_sysinit();
    /**********************从平台的到数据!!!*********************/
    get_zd_long ( "0440",   &dt             );   /*日      期*/
    get_zd_long ( "0450",   &dueDt          );   /*到期日    */
    get_zd_double ( "0390", &drftAmt        );   /*票面金额  */
    get_zd_double ( "0840", &intrstRate     );   /*利率      */
    get_zd_double ( "0420", &amt            );   /*实付金额  */
     
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "101F",   cTmpBuff       );   /*交易流水号*/
    zip_space(cTmpBuff);
    memcpy(tranSeqNb , cTmpBuff , sizeof(tranSeqNb)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0980",   cTmpBuff       );   /*交易流水号*/
    zip_space(cTmpBuff);
    memcpy(lw_ind , cTmpBuff , sizeof(lw_ind)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0640",   cTmpBuff        );   /*贴现类型  */
    zip_space(cTmpBuff);
    memcpy(dscntTyp , cTmpBuff , sizeof(dscntTyp)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0580",   cTmpBuff );   /*贴入行行号*/
    zip_space(cTmpBuff);
    memcpy(dscntInAcctSvcr , cTmpBuff , sizeof(dscntInAcctSvcr)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0810",   cTmpBuff     );   /*贴入行账号*/
    zip_space(cTmpBuff);
    memcpy(dscntInAcct , cTmpBuff , sizeof(dscntInAcct)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0961",   cTmpBuff       );   /*贴入行名称*/
    zip_space(cTmpBuff);
    memcpy(dscntInNm , cTmpBuff , sizeof(dscntInNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0600",   cTmpBuff);   /*贴出行行号*/
    zip_space(cTmpBuff);
    memcpy(dscntOutAcctSvcr , cTmpBuff , sizeof(dscntOutAcctSvcr)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0962",   cTmpBuff      );   /*贴出行名称*/
    zip_space(cTmpBuff);
    memcpy(dscntOutNm , cTmpBuff , sizeof(dscntOutNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0610",   cTmpBuff   );   /*(贴出)承接行行号*/
    zip_space(cTmpBuff);
    memcpy(agtAcctSvcr_1 , cTmpBuff , sizeof(agtAcctSvcr_1)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "1011",   cTmpBuff          );   /*清算标志  */
    zip_space(cTmpBuff);
    memcpy(stlFlg , cTmpBuff , sizeof(stlFlg)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0670",   cTmpBuff         );   /*票据分类  */
    zip_space(cTmpBuff);
    memcpy(drftCls , cTmpBuff , sizeof(drftCls)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0910",   cTmpBuff         );   /*票据类型  */
    zip_space(cTmpBuff);
    memcpy(drftTyp , cTmpBuff , sizeof(drftTyp)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "1032",   cTmpBuff          );   /*票据号    */
    zip_space(cTmpBuff);
    memcpy(drftNb , cTmpBuff , sizeof(drftNb)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0830",   cTmpBuff        );   /*出票人账号*/
    zip_space(cTmpBuff);
    memcpy(drwrAcct , cTmpBuff , sizeof(drwrAcct)-1);
    
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0963",   cTmpBuff          );   /*出票人名称*/
    zip_space(cTmpBuff);
    memcpy(drwrNm , cTmpBuff , sizeof(drwrNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0690",   cTmpBuff       );   /*利率类型  */
    zip_space(cTmpBuff);
    memcpy(intrstTyp , cTmpBuff , sizeof(intrstTyp)-1);
    
    vtcp_log("[%s][%d]日期--[%d]",__FILE__,__LINE__,dt);
    vtcp_log("[%s][%d]交易流水号--[%s]",__FILE__,__LINE__,tranSeqNb);
    vtcp_log("[%s][%d]贴现类型--[%s]",__FILE__,__LINE__,dscntTyp);
    vtcp_log("[%s][%d]贴入行行号--[%s]",__FILE__,__LINE__,dscntInAcctSvcr);
    vtcp_log("[%s][%d]贴入行账号--[%s]",__FILE__,__LINE__,dscntInAcct);
    vtcp_log("[%s][%d]贴入行名称--[%s]",__FILE__,__LINE__,dscntInNm);
    vtcp_log("[%s][%d]贴出行行号--[%s]",__FILE__,__LINE__,dscntOutAcctSvcr);
    vtcp_log("[%s][%d]贴出行账号--[%s]",__FILE__,__LINE__,dscntOutAcct);
    vtcp_log("[%s][%d]贴出行名称--[%s]",__FILE__,__LINE__,dscntOutNm);
    vtcp_log("[%s][%d](贴出)承接行行号--[%s]",__FILE__,__LINE__,agtAcctSvcr_1);
    vtcp_log("[%s][%d]清算标志--[%s]",__FILE__,__LINE__,stlFlg);
    vtcp_log("[%s][%d]票据分类--[%s]",__FILE__,__LINE__,drftCls);
    vtcp_log("[%s][%d]票据类型--[%s]",__FILE__,__LINE__,drftTyp);
    vtcp_log("[%s][%d]票据号--[%s]",__FILE__,__LINE__,drftNb);
    vtcp_log("[%s][%d]票面金额--[%lf]",__FILE__,__LINE__,drftAmt);
    vtcp_log("[%s][%d]到期日--[%d]",__FILE__,__LINE__,dueDt);
    vtcp_log("[%s][%d]利率类型--[%s]",__FILE__,__LINE__,intrstTyp);
    vtcp_log("[%s][%d]利率--[%lf]",__FILE__,__LINE__,intrstRate);
    vtcp_log("[%s][%d]实付金额--[%lf]",__FILE__,__LINE__,amt);
    vtcp_log("[%s][%d]出票人账号--[%s]",__FILE__,__LINE__,drwrAcct);
    get_zd_data("0030",g_pub_tx.tx_br_no);
    get_zd_data("0020",g_pub_tx.opn_br_no);
	vtcp_log("[%s][%d]出票人名称--[%s]",__FILE__,__LINE__,drwrNm);     
    vtcp_log("[%s][%d]交易机构--[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no);     

    vtcp_log("[%s][%d] drftNb=[%s]  drftTyp=[%s]", __FILE__,__LINE__,drftNb,drftTyp);
    
    if(!memcmp(drftTyp,"AC01",sizeof(drftTyp)-1))
    {
        memset(drftTyp,0 ,sizeof(drftTyp));
        memcpy(drftTyp,"111",3);
    }
    else if (!memcmp(drftTyp,"AC02",sizeof(drftTyp)-1))
    {
        memset(drftTyp,0 ,sizeof(drftTyp));
        memcpy(drftTyp,"112",3);
    }
    else 
    {
        sprintf(acErrMsg, "类型错误,请检查[%s]",drftTyp);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P051");
        goto ErrExit;
    }
    
    ret = sql_count("ecds_recv_reg","ecds_tx_date = %ld and ecds_trace_no ='%s' ",dt,tranSeqNb);
    if (ret!=0)
    {
        vtcp_log("[%s][%d] 已经发过此笔报文信息ret[%d]",__FILE__,__LINE__,ret);
        WRITEMSG
        
        ret = Ecds_recv_reg_Dec_Upd(g_pub_tx.reply,"ecds_tx_date = %ld and ecds_trace_no ='%s' and bf_flag='1' ",dt,tranSeqNb);
        if (ret)
        {
            sprintf(acErrMsg, "执行Ecds_recv_reg_Dec_Upd错[%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        ret = Ecds_recv_reg_Fet_Upd(&s_Ecds_recv_reg, g_pub_tx.reply);/***没有找到等于1的,那就说明等于0或者2**/
        if (ret) {
           sprintf(acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret);
           WRITEMSG
           strcpy(g_pub_tx.reply, "NXDP");
           goto ErrExit;
        }
        s_Ecds_recv_reg.bf_flag[0]='2';/**更新补发标志为正式接受**/
        ret = Ecds_recv_reg_Upd_Upd(s_Ecds_recv_reg, g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg, "执行Ecds_recv_reg_Dec_Upd错[%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        Ecds_recv_reg_Clo_Upd();
    }
    else
    {
        vtcp_log("[%s][%d]----", __FILE__,__LINE__);
        /**************准备插入ECDS_RECV_REG登记簿****************/
        sEcds_recv_reg.issedt       = 0 ;
        sEcds_recv_reg.duedt        = dueDt;
        sEcds_recv_reg.ecds_tx_date = dt;
        sEcds_recv_reg.trace_no     = 0;
        sEcds_recv_reg.tx_date      = g_pub_tx.tx_date;
        
        sEcds_recv_reg.sts[0]='1';
        sEcds_recv_reg.check_flag[0]='1';
        
        memcpy(sEcds_recv_reg.tag_name,         "4956",     sizeof( sEcds_recv_reg.tag_name)-1);
        memcpy(sEcds_recv_reg.ecds_trace_no,    tranSeqNb,  sizeof(sEcds_recv_reg.ecds_trace_no)-1);
        memcpy(sEcds_recv_reg.drftcls,          drftCls,    sizeof(sEcds_recv_reg.drfttyp)-1);
        memcpy(sEcds_recv_reg.drfttyp ,         drftTyp,    sizeof(sEcds_recv_reg.drftnb)-1);
        memcpy(sEcds_recv_reg.drftnb ,          drftNb,     sizeof(sEcds_recv_reg.drftnb)-1);   
        
        Ecds_recv_reg_Debug(&sEcds_recv_reg);
        ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
        if (ret )
        {
            sprintf(acErrMsg, "插入Ecds_recv_reg错!!ret=[%d]", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "NXC8");
            goto ErrExit;
        }
    }
    ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab1," or_br_no= '%s' ",dscntInAcctSvcr);
    if (ret)
    {
        vtcp_log("查询Hv_orno_brno表错误!! ret [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab2," or_br_no= '%s' ",dscntOutAcctSvcr);
    if (ret)
    {
        vtcp_log("查询Hv_orno_brno表错误!! ret [%d]",ret);
        WRITEMSG 
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    /************根据判断，贴入行的处理************/
    /*if(memcmp(sHv_uniontab1.qs_no,HV_QS_BR_NO,12)==0 && memcmp(sHv_uniontab2.qs_no,HV_QS_BR_NO,12)!=0)modify by ligl 20100506**/
    if(memcmp(sHv_uniontab1.qs_no,HV_QS_BR_NO,12)==0 && memcmp(sHv_uniontab2.or_br_no,sHv_uniontab1.or_br_no,12)!=0 && lw_ind[0]=='1')
    {
        ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt," pnote_no= '%s' and  note_type ='%s' and sts='X' ", drftNb,drftTyp);     
        if (ret)
        {
            sprintf(acErrMsg,"查询mo_discnt表错误!!ret= [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B112" );
            goto ErrExit;
        }
        if(sMo_discnt.po_knd[0]!='E')
        {
            sprintf(acErrMsg,"非电票不能用此业务=[%s]",sMo_discnt.po_knd);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B112" );
            goto ErrExit;
        }
	/**add by ligl 20100506 yq***/
        if(memcmp(sHv_uniontab1.qs_no,sHv_uniontab2.qs_no,12)==0)
	{
	   if(memcmp(stlFlg,"SM00",4)==0)
	   {
		strcpy(g_pub_tx.reply,"B121");
	 	goto ErrExit;	
	   }
	} 
        /**add end by ligl 20100506 yq***/
        GetBr_noByOr_br_no(dscntInAcctSvcr,cKinbr);
        vtcp_log("[%s][%d] cKinbr=[%s] ",__FILE__,__LINE__,cKinbr);
        
        if(memcmp(cKinbr,sMo_discnt.br_no,BRNO_LEN)!=0)
        {
            sprintf(acErrMsg,"非我行票据=[%s][%s]",cKinbr,sMo_discnt.br_no);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "Z10D" );
            goto ErrExit;
        }
        set_zd_data("0910" , cKinbr);
        
        /***设置开户机构和交易机构***/
        set_zd_data("0020", cKinbr);
        set_zd_data("0030", cKinbr);
        ret = Com_parm_Sel (g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn= %d ",cKinbr,PARM_ECDS_TC_AC);
        if (ret)
        {
            sprintf(acErrMsg, "查询Com_parm表错误!!ret  [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        vtcp_log("[%s][%d] sCom_parm.val=[%s] ",__FILE__,__LINE__,sCom_parm.val);
        /***********************为A017准备的数据********************************/
        set_zd_data("0300",sCom_parm.val);
        /***********************B921,转贴现录入***********************************/
        set_zd_long("0480",0);                                 /* 在途天数 */
        set_zd_data("0690","1");                               /* 操作码  */
        set_zd_data("0620",sMo_discnt.pact_no);                /* 转贴现协议编号*/
        set_zd_long("0450",sMo_discnt.not_matr_date);          /* 汇票到期日*/
        set_zd_data("0310",sMo_discnt.rem_ac_no);              /* 出票人帐号*/
        set_zd_data("0260",sMo_discnt.remitter);               /* 出票人全称*/
        set_zd_data("0810",sMo_discnt.payer_brno_name);        /* 付款行全称*/    
        set_zd_data("0320",sMo_discnt.payee_ac_no);            /* 收款人帐号*/
        set_zd_data("0270",sMo_discnt.payee);                  /* 收款人全称*/
        set_zd_data("0820",sMo_discnt.payee_brno_name);        /* 收款人开户行全称*/
        set_zd_data("0900",sMo_discnt.note_type);              /* 票据类型 */
        set_zd_data("0980",drftCls);/*电票标志*/
        set_zd_data("0600",sMo_discnt.pnote_no);               /* 票据号   */
        set_zd_data("0680",sMo_discnt.local_ind);              /* 异地同城标志*/
        set_zd_data("0250",sMo_discnt.pn_name);                /* 申请人名称*/
	      sMo_discnt.tpn_int=sMo_discnt.par_amt-amt; 
        set_zd_double("1001",sMo_discnt.tpn_int);              /* 汇票金额*/
        set_zd_double("1002",sMo_discnt.par_amt);              /* 汇票金额*/
        set_zd_double("0840",sMo_discnt.pn_rate);              /* 转贴现率*/
        if ( memcmp( dscntTyp, "RM00",4 )==0)
        {
            set_zd_data("0990","2");               /* 贴现类型 2:买断式   */
        }
        else if(memcmp( dscntTyp, "RM01",4 )==0)
        {
            set_zd_data("0990","3");               /* 贴现类型 3:回购式   */
        }
        else 
        {
            sprintf(acErrMsg,"信息错误,请检查[%s]",dscntTyp);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P051");
            goto ErrExit;
        }
        /***********************B922,转贴现记账***********************************/
        set_zd_data("0700", "1");           /* 收款账户类型 XXXXXX*/ 

        strcpy ( g_pub_tx.reply , "0001" );
        goto GoodExit1; 
    }
    /***else if(memcmp(sHv_uniontab2.qs_no,HV_QS_BR_NO,12)==0 && memcmp(sHv_uniontab1.qs_no,HV_QS_BR_NO,12)!=0)**modify by ligl 20100506 **//**贴出行的处理**/
    else if(memcmp(sHv_uniontab2.qs_no,HV_QS_BR_NO,12)==0 && memcmp(sHv_uniontab1.or_br_no,sHv_uniontab2.or_br_no,12)!=0 && lw_ind[0]=='2')/**贴出行的处理**/
    {
       
        ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt," pnote_no= '%s' and  note_type ='%s' and sts='3' ", drftNb,drftTyp);     
        if (ret)
        {
            sprintf(acErrMsg,"查询mo_discnt表错误!!ret= [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B112" );
            goto ErrExit;
        }
        GetBr_noByOr_br_no(dscntOutAcctSvcr,cKinbr);
        vtcp_log("[%s][%d] cKinbr=[%s] br_no=[%s]",__FILE__,__LINE__,cKinbr,sMo_discnt.br_no);
        if(memcmp(cKinbr,sMo_discnt.br_no,BRNO_LEN)!=0)
        {
            sprintf(acErrMsg,"查询mo_discnt表错误!!ret= [%d]cKinbr[%s]sMo_discnt.br_no[%s]",ret,cKinbr,sMo_discnt.br_no);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B305" );
            goto ErrExit;
        }
	/***add by ligl 20100506 yq**/
        if(memcmp(sHv_uniontab1.qs_no,sHv_uniontab2.qs_no,12)==0)
	{
	   if(memcmp(stlFlg,"SM00",4)==0)
	   {
		strcpy(g_pub_tx.reply,"B121");
	 	goto ErrExit;	
	   }
	} /**end***/
        set_zd_data("0910" , cKinbr);
        
        /***设置开户机构和交易机构***/
        set_zd_data("0020", cKinbr);
        set_zd_data("0030", cKinbr);
     	        char tx_tel[5];
                memset(tx_tel,0x00,sizeof(tx_tel));
                memcpy(tx_tel,cKinbr+1,2);
                memcpy(tx_tel+2,"99",2);
                strcpy(g_pub_tx.tel,tx_tel);
  	set_zd_data("0070",g_pub_tx.tel); 
        ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_code='%s'  and parm_seqn= %d ",cKinbr,PARM_ECDS_TR_PAY_AC);
        if (ret)
        {
            sprintf(acErrMsg, "查询Com_parm表错误!!ret  [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        
        
        /********************如何编造一个转贴现协议编号************************/
        char cTmpBuff[9];
        char cTraceNo[10];
        memset(cTmpBuff , 0 , sizeof(cTmpBuff));
        sprintf(cTmpBuff ,"%08d" , g_pub_tx.tx_date);
        vtcp_log("[%s][%d] cTmpBuff=[%s] ",__FILE__,__LINE__,cTmpBuff);
        
        /*sprintf(cTpact_no ,"%08d" , g_pub_tx.tx_date);**/
        memcpy(cTpact_no , cTmpBuff+2 , 6);
        vtcp_log("[%s][%d] cTpact_no=[%s] ",__FILE__,__LINE__,cTpact_no);
        strcat(cTpact_no , sMo_discnt.br_no );
        vtcp_log("[%s][%d] cTpact_no=[%s] trace_no=[%ld]",__FILE__,__LINE__,cTpact_no,g_pub_tx.trace_no);
        
        sprintf(cTraceNo,"%09d",g_pub_tx.trace_no);
        strcat(cTpact_no , cTraceNo );
        vtcp_log("[%s][%d]转贴现协议编号=[%s]", __FILE__,__LINE__,cTpact_no);

        /************************为B931准备数据**************************/
        set_zd_data("0690","0");/*操作码   0录入1修改2删除*/
        set_zd_double("0840",sMo_discnt.tpn_rate);/*转贴现率*/
        set_zd_data("0900",sMo_discnt.note_type);/*票据类型*/
        set_zd_data("0600",sMo_discnt.pnote_no);/*票据号*/
        set_zd_data("0680",sMo_discnt.local_ind);/*异地同城标志*/
        set_zd_data("0980",drftCls);/*电票标志*/
        set_zd_data("0250",sMo_discnt.tpn_name);/*对方行名称*/
        set_zd_data("0380",cTpact_no);
	    sMo_discnt.tpn_int=sMo_discnt.par_amt-amt; 
        set_zd_double("1001",sMo_discnt.tpn_int);/*转贴现利息*/
        set_zd_data("0720",sMo_discnt.tpn_style);/*追索权0：无1：有*/
        /*set_zd_long("1261",sMo_discnt.tt_no);*/
        vtcp_log("[%s][%d]----进入贴出行买断式cKinbr=%s", __FILE__,__LINE__,cKinbr);
        
        vtcp_log("[%s][%d] dscntTyp=[%s] ",__FILE__,__LINE__,dscntTyp);
        if ( memcmp( dscntTyp, "RM00",4 )==0)/*是贴出行买断式*/
        {
            /************************为L201准备数据**************************/
            
            vtcp_log("[%s][%d] 贷款ac_id=[%ld]",__FILE__,__LINE__,sMo_discnt.ac_id);
            ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_id=%ld ", sMo_discnt.ac_id);
            if (ret) {
               sprintf(acErrMsg, "查询mdm_ac_rel出错![%s]", sMo_discnt.ac_id);
               WRITEMSG
               strcpy(g_pub_tx.reply, "L015");
               goto ErrExit;
            }
            set_zd_data("0330",sMdm_ac_rel.ac_no);
            vtcp_log("[%s][%d] 贷款账户=[%s]",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
            
            /************************为B932准备数据**************************/
            set_zd_data("0900", sMo_discnt.note_type);    /* 票据类型 */
            set_zd_data("0600", sMo_discnt.pnote_no);     /* 票据号 */
            set_zd_data("0980", drftCls);     /* 票据号 */
            set_zd_data("0370", sMo_discnt.tin_ac_no);    /* 转贴现帐号 */
            set_zd_data("0670", sMo_discnt.tpay_ac_type); /* 付款账户类型 */
	    sMo_discnt.tpn_int=sMo_discnt.par_amt-amt; 
            set_zd_double("1001",sMo_discnt.tpn_int);     /* 转贴现利息 */
            set_zd_double("1002", sMo_discnt.par_amt);    /* 汇票金额 */
            set_zd_data("0700", "1"); /* 付款账户类型 */
            /***********************为A016准备*****************/
            set_zd_data("112A",sCom_parm.val);
            set_zd_data("0990","4");               /* 转贴现转出 4:买断式  */
        
            strcpy ( g_pub_tx.reply , "0002" );
            goto GoodExit1;
        }
        else if( memcmp( dscntTyp, "RM01",4 ) ==0 )/*是贴出行回购式*/
        {
            vtcp_log("[%s][%d]----进入贴出行回购式", __FILE__,__LINE__);
            /**********************为B947准备数据****************************/
            set_zd_data("0900", sMo_discnt.note_type);   /* 票据类型 */
            set_zd_data("0600", sMo_discnt.pnote_no);    /* 票据号 */
            set_zd_data("0980", drftCls);    /* 票据号 */
            set_zd_data("0670", sMo_discnt.tpay_ac_type);/* 付款账户类型 */
	    sMo_discnt.tpn_int=sMo_discnt.par_amt-amt; 
            set_zd_double("1001", sMo_discnt.tpn_int);        /* 转贴现利息 */
            set_zd_double("1002", drftAmt);              /* 票据面额 */
            /***********************为A016准备*****************/
            set_zd_data("0300",sCom_parm.val);
            set_zd_data("0990","5");               /* 转贴现转出 6:回购式   */
            strcpy ( g_pub_tx.reply , "0003" );
            goto GoodExit1;
        } 
        else 
        {
            sprintf(acErrMsg,"信息错误,请检查[%s]",dscntTyp);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P051");
            goto ErrExit;
        }
    }
    else
    {
        sprintf(acErrMsg,"信息错误,请检查[%s][%s]",sHv_uniontab1.qs_no,sHv_uniontab2.qs_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P051");
        goto ErrExit;
    }
    vtcp_log("[%s][%d] tx_br_no=[%s] opn_br_no=[%s][%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no,cKinbr);
GoodExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
GoodExit1:
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
        memcpy(g_pub_tx.reply,"B556" ,4);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
