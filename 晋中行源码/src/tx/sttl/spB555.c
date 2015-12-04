/*************************************************
* 文 件 名:  spB555.c   贴现赎回通知 
* 功能描述:
*    做为贴入行，收到贴现赎回通知后
*    贴入行核心系统进行账务处理，并处理贴现登记簿。                    
* 作    者:  chenchao
* 完成日期： 2010-4-7 20:30:43
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
insert into tx_def values ('4955','贴现赎回通知','10000000000000000000000000000000000000000100000000000000010000000000000000000000111000000000000000000000000000000000000000000000','1','3','2','3',null)
insert into tx_flow_def  values ('4955','0','B555','#=0001@1@#$')
insert into tx_flow_def values ('4955', 1, 'B914', '#=0001@2@#=0002@3@#$')
insert into tx_flow_def values ('4955', 2, 'a016', '#=0000@5@#$')
insert into tx_flow_def values ('4955', 3, 'D003', '#=0000@5@#$')
insert into tx_flow_def values ('4955',5 , 'L201', '#=0000@6@#$')
insert into tx_flow_def values ('4955',6 , 'L320', '#=0001@7@#$')
insert into tx_flow_def values ('4955',7 , 'A016', '#=0000@8@#$')
insert into tx_flow_def values ('4955',8 , 'A017', '#$')
insert into tx_sub_def values ('B555','贴现赎回通知','spB555','0','0')

insert into tx_sub_rel values ('4955','D003','0','0300','1021',null)                 
insert into tx_sub_rel values ('4955','D003','0','0890','1023','凭证类型')           
insert into tx_sub_rel values ('4955','D003','0','0580','1024','凭证号')             
insert into tx_sub_rel values ('4955','D003','0','0400','102F',null)                 
insert into tx_sub_rel values ('4955','D003','0','0680','102K', null )  
insert into tx_sub_rel values ('4955','D003','0','0200','0191', null ) 
insert into tx_sub_rel values ('4955','D003','0','0240','0192', null ) 

insert into tx_sub_rel values ('4955','D252','1','01','1204',null)                   
insert into tx_sub_rel values ('4955','D252','1','2','1205',null)                    
insert into tx_sub_rel values ('4955','D252','0','1002','1208',null)                 
insert into tx_sub_rel values ('4955','D252','1','贴现还款','120A',null)             
insert into tx_sub_rel values ('4955','J003','0','0330','1231',null)                 
insert into tx_sub_rel values ('4955','J003','0','1002','1232',null)                 
insert into tx_sub_rel values ('4955','L201','1','01','0210',null)                   
insert into tx_sub_rel values ('4955','L201','1','9','0660',null)                    
insert into tx_sub_rel values ('4955','L201','0','0330','1071',null)                 
insert into tx_sub_rel values ('4955','L201','1','0','1072',null)                    
insert into tx_sub_rel values ('4955','L201','1','1','107A',null)                    
insert into tx_sub_rel values ('4955','L201','0','1002','107B',null)                 
insert into tx_sub_rel values ('4955','a016','0','0300','1201',null)                 
insert into tx_sub_rel values ('4955','a016','1','2','1205',null)                    
insert into tx_sub_rel values ('4955','a016','0','0890','1206','凭证类型')           
insert into tx_sub_rel values ('4955','a016','0','0580','1207','凭证号')             
insert into tx_sub_rel values ('4955','a016','0','1002','1208',null)                 
insert into tx_sub_rel values ('4955','a016','1','贴现还款记账','120A',null)

insert into dc_entry values ('B55500','2','01','12912','2','1','1','2','1001','贴现利息调整')
insert into dc_entry values ('B55500','3','01','71200','1','1','1','2','1002','贴现票据')
insert into tx_dc_rel values ('B555','0000','B55500')
*************************************************/
#define ERR_DEAL if( ret ) \
{\
    sprintf( acErrMsg, "sql error" ); \
    WRITEMSG \
    goto ErrExit; \
}
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "ln_mst_c.h"
#include "hv_uniontab_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"

int 
spB555()
{
    struct mo_bank_acc_po_c sMo_bank_acc_po;
    struct ecds_recv_reg_c    sEcds_recv_reg,sEcdsrecvreg; 
    struct hv_uniontab_c    s_hv_uniontab;  
    struct mo_discnt_c    sMo_discnt;
    struct mdm_ac_rel_c    sMdm_ac_rel;
    struct hv_uniontab_c    shv_uniontab;
    struct com_parm_c    sCom_parm;

    /*接受的报文内容*/
    long     dt=0;                    /*　日期  */ 
    long     dueDt=0;                                  /*到期日            */
    char     intrstTyp[2];                             /*利率类型          */
    char     stlFlg[5];                               /*清算标志          */
    char     drftCls[2];                              /*票据分类          */
    char     drftTyp[5];                              /*票据类型          */
    char     drftNb[31];                              /*票据号            */
    char     tranSeqNb[29];                           /*交易流水号        */
    char     origDscntInNm[61];                       /*原贴入行名称      */
    char     agtAcctSvcr_1[13];                       /*(原贴入)承接行行号*/
    char     origDscntOutNm[61];                      /*原贴出行名称      */
    char     agtAcctSvcr_2[13];                       /*(原贴出)承接行行号*/
    char     origDscntInAcct[33];                    /*原贴入行账号      */
    char     origDscntOutAcct[33];                    /*原贴出人账号      */
    char     origDscntInAcctSvcr[13];                 /*原贴入行行号      */
    char     origDscntOutAcctSvcr[13];                /*原贴出人开户行    */
    
             
    double    drftAmt=0.00;                             /*票面金额          */
    double    intrstRate=0.00;                           /*利率              */           
    double    amt=0.00;                                 /*实付金额          */     
    /*end*/

    char        cTmpBuff[101];
    char        cBr_no[7];
    long        l_date=0;
    int         ttlnum = 0;    /**读取的总条数**/
    int         cnt=0;
    int         i = 0;
    int         ret = 0,pub_ret=0,ins_flag=0;
    vtcp_log("%s.%d", __FILE__,__LINE__);
    memset(&sMo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&shv_uniontab,0x00,sizeof(struct hv_uniontab_c));
    memset(&s_hv_uniontab,0x00,sizeof(struct hv_uniontab_c));
    memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sMo_discnt,0x00,sizeof(struct mo_discnt_c));
    memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
    memset(&sEcdsrecvreg,0x00,sizeof(struct ecds_recv_reg_c));
    
    memset(stlFlg                       ,0x00,sizeof( stlFlg               ));
    memset(drftCls                      ,0x00,sizeof( drftCls              ));
    memset(drftTyp                      ,0x00,sizeof( drftTyp              ));
    memset(drftNb                       ,0x00,sizeof( drftNb               ));
    memset(tranSeqNb                    ,0x00,sizeof( tranSeqNb            ));
    memset(intrstTyp                    ,0x00,sizeof( intrstTyp            ));
    memset(origDscntInAcct              ,0x00,sizeof( origDscntInAcct      ));
    memset(origDscntInAcctSvcr          ,0x00,sizeof( origDscntInAcctSvcr  ));
    memset(origDscntInNm                ,0x00,sizeof( origDscntInNm        ));
    memset(agtAcctSvcr_1                ,0x00,sizeof( agtAcctSvcr_1        ));
    memset(origDscntOutAcct             ,0x00,sizeof( origDscntOutAcct     ));
    memset(origDscntOutAcctSvcr         ,0x00,sizeof( origDscntOutAcctSvcr ));
    memset(origDscntOutNm               ,0x00,sizeof( origDscntOutNm       ));
    memset(agtAcctSvcr_2                ,0x00,sizeof( agtAcctSvcr_2        ));
    
    vtcp_log("%s.%d", __FILE__,__LINE__);
    pub_base_sysinit();

    get_zd_data("0030",cBr_no);

    get_zd_long("0440",&dt);            /*日期*/
    get_zd_double("0840",&intrstRate);        /*利率*/
    get_zd_double("0420",&amt);            /*实付金额*/
    get_zd_double("0390",&drftAmt);            /*票面金额*/
    get_zd_long("0450",&dueDt);            /*到期日*/
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_char("101F",cTmpBuff);            /*交易流水号*/
    zip_space(cTmpBuff);
    memcpy(tranSeqNb , cTmpBuff , sizeof(tranSeqNb)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0810",cTmpBuff);        /*原贴入行账号*/
    zip_space(cTmpBuff);
    memcpy(origDscntInAcct , cTmpBuff , sizeof(origDscntInAcct)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0630",cTmpBuff);    /*原贴入行行号*/
    zip_space(cTmpBuff);
    memcpy(origDscntInAcctSvcr , cTmpBuff , sizeof(origDscntInAcctSvcr)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0961",cTmpBuff);        /*原贴入行名称*/
    zip_space(cTmpBuff);
    memcpy(origDscntInNm , cTmpBuff , sizeof(origDscntInNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0590",cTmpBuff);        /*(原贴入)承接行行号*/
    zip_space(cTmpBuff);
    memcpy(agtAcctSvcr_1 , cTmpBuff , sizeof(agtAcctSvcr_1)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0820",cTmpBuff);        /*原贴出人账号*/
    zip_space(cTmpBuff);
    memcpy(origDscntOutAcct , cTmpBuff , sizeof(origDscntOutAcct)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0600",cTmpBuff);            /*原贴出人开户行*/
    zip_space(cTmpBuff);
    memcpy(origDscntOutAcctSvcr , cTmpBuff , sizeof(origDscntOutAcctSvcr)-1);
    
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0962",cTmpBuff);        /*原贴出行名称*/
    zip_space(cTmpBuff);
    memcpy(origDscntOutNm , cTmpBuff , sizeof(origDscntOutNm)-1);
    
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0610",cTmpBuff);        /*(原贴出)承接行行号*/
    zip_space(cTmpBuff);
    memcpy(agtAcctSvcr_2 , cTmpBuff , sizeof(agtAcctSvcr_2)-1);
    
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("101G",cTmpBuff);            /*清算标志*/
    zip_space(cTmpBuff);
    memcpy(stlFlg , cTmpBuff , sizeof(stlFlg)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0670",cTmpBuff);            /*票据分类*/
    zip_space(cTmpBuff);
    memcpy(drftCls , cTmpBuff , sizeof(drftCls)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0910",cTmpBuff);            /*票据类型*/
    zip_space(cTmpBuff);
    memcpy(drftTyp , cTmpBuff , sizeof(drftTyp)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("1032",cTmpBuff);            /*票据号*/
    zip_space(cTmpBuff);
    memcpy(drftNb , cTmpBuff , sizeof(drftNb)-1);
    
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0690",cTmpBuff);            /*利率类型*/
    zip_space(cTmpBuff);
    memcpy(intrstTyp , cTmpBuff , sizeof(intrstTyp)-1);
    
    
    
    vtcp_log("%s.%d", __FILE__,__LINE__);
    vtcp_log("[%s][%d]日期[%d]",__FILE__,__LINE__,dt);
    vtcp_log("[%s][%d]交易流水号[%s]",__FILE__,__LINE__,tranSeqNb);
    vtcp_log("[%s][%d]原贴入行账号[%s]",__FILE__,__LINE__,origDscntInAcct);
    vtcp_log("[%s][%d]原贴入行行号[%s]",__FILE__,__LINE__,origDscntInAcctSvcr);
    vtcp_log("[%s][%d]原贴入行名称[%s]",__FILE__,__LINE__,origDscntInNm);
    vtcp_log("[%s][%d](原贴入);承接行行号[%s]",__FILE__,__LINE__,agtAcctSvcr_1);
    vtcp_log("[%s][%d]原贴出人账号[%s]",__FILE__,__LINE__,origDscntOutAcct);
    vtcp_log("[%s][%d]原贴出人开户行[%s]",__FILE__,__LINE__,origDscntOutAcctSvcr);
    vtcp_log("[%s][%d]原贴出行名称[%s]",__FILE__,__LINE__,origDscntOutNm);
    vtcp_log("[%s][%d](原贴出);承接行行号[%s]",__FILE__,__LINE__,agtAcctSvcr_2);
    vtcp_log("[%s][%d]清算标志[%s]",__FILE__,__LINE__,stlFlg);
    vtcp_log("[%s][%d]票据分类[%s]",__FILE__,__LINE__,drftCls);
    vtcp_log("[%s][%d]票据类型[%s]",__FILE__,__LINE__,drftTyp);
    vtcp_log("[%s][%d]票据号[%s]",__FILE__,__LINE__,drftNb);
    vtcp_log("[%s][%d]票面金额[%lf]",__FILE__,__LINE__,drftAmt);
    vtcp_log("[%s][%d]到期日[%d]",__FILE__,__LINE__,dueDt);
    vtcp_log("[%s][%d]利率类型[%s]",__FILE__,__LINE__,intrstTyp);
    vtcp_log("[%s][%d]利率[%lf]",__FILE__,__LINE__,intrstRate);
    vtcp_log("[%s][%d]实付金额[%lf]",__FILE__,__LINE__,amt);
    
    ret = Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsrecvreg,"ecds_tx_date = %ld and ecds_trace_no ='%s'",dt , tranSeqNb);
    if(ret == 0)
    {
        if(sEcdsrecvreg.bf_flag[0] == '0' || sEcdsrecvreg.bf_flag[0] == '2')
        {
        	sprintf(acErrMsg,"该笔票据已经处理!! sEcdsrecvreg.bf_flag=[%s]",sEcdsrecvreg.bf_flag);
        	WRITEMSG
        	strcpy ( g_pub_tx.reply , "NXDP" );
        	goto ErrExit;
        }
        else if(sEcdsrecvreg.bf_flag[0] == '1')
        {
        	pub_ret=sql_execute("update ecds_recv_reg  set bf_flag='2' where ecds_tx_date = %ld and ecds_trace_no ='%s' and bf_flag='1' ",dt , tranSeqNb);
        	if(pub_ret)
        	{
        		sprintf(acErrMsg, "update ecds_recv_reg error ,pub_ret [%d]", pub_ret);
              		WRITEMSG
              		strcpy(g_pub_tx.reply,"D104");
              		goto ErrExit;
              	}   
              	ins_flag=1;
              	
        }
        else
        {
        	sprintf(acErrMsg,"补发标志错误,检查[%s]",sEcdsrecvreg.bf_flag);
        	WRITEMSG
        	strcpy ( g_pub_tx.reply , "D999" );
        	goto ErrExit;
        }
    }
    else if (ret && ret !=100)
    {
    	sprintf(acErrMsg,"查询Ecds_recv_reg表错误!! [%s]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT03" );
        goto ErrExit;
    }
    if(!memcmp(drftTyp,"AC01",sizeof(drftTyp)-1))
    {
        memset(drftTyp,0x00,sizeof(drftTyp));
        memcpy(drftTyp,"111",3);
    }
    else if (!memcmp(drftTyp,"AC02",sizeof(drftTyp)-1))
    {
        memset(drftTyp,0x00,sizeof(drftTyp));
        memcpy(drftTyp,"112",3);
    }
    else {
        sprintf(acErrMsg, "类型错误  请检查");
        WRITEMSG
        goto ErrExit;
    }
    ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt," pnote_no= '%s' and  note_type ='%s' and sts='3' ", drftNb , drftTyp);     
    if (ret)
    {
        sprintf(acErrMsg, "查询mo_discnt表错误!!ret= [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B112" );
        goto ErrExit;
    }
    ret = Mdm_ac_rel_Sel (g_pub_tx.reply,&sMdm_ac_rel ," ac_id =%ld ",sMo_discnt.ac_id);
    if (ret)
    {
        sprintf(acErrMsg, "查询Mdm_ac_rel表错误!!ret= [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "W011" );
        goto ErrExit;
    }
    ret = Hv_uniontab_Sel(g_pub_tx.reply,&shv_uniontab," or_br_no='%s' ",origDscntInAcctSvcr);
    if (ret)
    {
        sprintf(acErrMsg, "查询Hv_orno_brno表错误!! ret=[%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    ret = Hv_uniontab_Sel(g_pub_tx.reply,&s_hv_uniontab," or_br_no= '%s' ",origDscntOutAcctSvcr);
    if (ret)
    {
        sprintf(acErrMsg, "查询Hv_orno_brno表错误!! ret [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    vtcp_log("%s.%d----", __FILE__,__LINE__);
    if (memcmp(shv_uniontab.qs_no ,s_hv_uniontab.qs_no ,sizeof(shv_uniontab.qs_no)-1))/*不同清算行*/
    {
        ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm,"parm_seqn= %d and parm_code='%s' ",PARM_ECDS_TR_PAY_AC,cBr_no);
        if (ret)
        {
            sprintf(acErrMsg, "查询Com_parm表错误!!ret  [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
    }
    else
    {
        if (!memcmp(stlFlg,"SM00",sizeof(stlFlg)-1 ) ) /*同一清算行 不允许线上交易*/
        {
            sprintf(acErrMsg, "同一清算行不能做线上清算");
            WRITEMSG
            strcpy ( g_pub_tx.reply , "P068" );
            goto ErrExit;
        }
        ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_seqn= %d and parm_code='%s' ",PARM_ECDS_TC_AC,cBr_no);
        if (ret)
        {
            sprintf(acErrMsg, "查询Com_parm表错误!!ret  [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
    }
    if(ins_flag == 1)
    {
    	sEcds_recv_reg.ecds_tx_date            =dt;
    	sEcds_recv_reg.trace_no         = 0;
    	sEcds_recv_reg.tx_date          = g_pub_tx.tx_date;
    	sEcds_recv_reg.issedt             = 0 ;
    	sEcds_recv_reg.duedt             = dueDt ;
    	sEcds_recv_reg.sts[0]='1';
    	sEcds_recv_reg.bf_flag[0]='0';
    	sEcds_recv_reg.check_flag[0]='1'; 
    	memcpy(sEcds_recv_reg.ecds_trace_no,    tranSeqNb,sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
    	memcpy(sEcds_recv_reg.drftcls,        drftCls,sizeof(sEcds_recv_reg.drfttyp)-1);     
    	memcpy(sEcds_recv_reg.drfttyp ,        drftTyp,    sizeof(sEcds_recv_reg.drftnb)-1);     
    	memcpy(sEcds_recv_reg.drftnb ,        drftNb,    sizeof(sEcds_recv_reg.drftnb)-1);   
    	memcpy(sEcds_recv_reg.ecds_trace_no,    tranSeqNb,sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
    	memcpy(sEcds_recv_reg.tag_name,        "CBS1000070",sizeof( sEcds_recv_reg.tag_name)-1);
    	
    	
    	ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
    	if (ret )
    	{
    	    Ecds_recv_reg_Debug(&sEcds_recv_reg);
    	    sprintf(acErrMsg, "插入Ecds_recv_reg错!!ret=[%d]", ret);
    	    WRITEMSG
    	    strcpy(g_pub_tx.reply, "NXC8");
    	    goto ErrExit;
    	}
    }
    
#if 0
    /*对利息进行记账*/
    memcpy(g_pub_tx.ac_get_ind,"000",3);
    g_pub_tx.tx_amt1=amt-drftAmt ;
    strcpy(g_pub_tx.ac_wrk_ind, "000000");
    memcpy(g_pub_tx.ac_no, "13060102",sizeof(g_pub_tx.ac_no)-1);      /* 账户 */      
    strcpy(g_pub_tx.ac_id_type, "1");    /* 账号类型 3-贷款 */
    strcpy(g_pub_tx.intst_type, "0");    /* 计息类型 */
    strcpy(g_pub_tx.add_ind, "0");    /* 增减:0减1加2销户 */
    strcpy(g_pub_tx.prt_ind, "0");    /* 是否打印存折(有折无折:1是0否 */
    strcpy(g_pub_tx.hst_ind, "1");    /* 入明细:1日间入2日终单 日终汇总入 */
    /*g_pub_tx.svc_ind = 9001;*/
    strcpy(g_pub_tx.brf, "贴现利息");
    sprintf(acErrMsg, "Pub_tx结构赋值 PASS!");
    WRITEMSG

    /*** 交易记帐处理 ***/
    if (pub_acct_trance()) 
    {
        sprintf(acErrMsg, "交易记帐处理错误!");
        WRITEMSG
        goto ErrExit;
    } 
    sprintf(acErrMsg, "交易记帐处理 PASS!");
    WRITEMSG
    set_zd_data("0210", "01"); 
    set_zd_double("1001", amt-drftAmt);
    set_zd_data("0660", "2");
    ret = pubf_acct_proc("B913");
    if (ret != 0) 
    {
        vtcp_log("会计记帐不成功[%s][%ld]",__FILE__,__LINE__);
        sprintf(acErrMsg, "会计记帐不成功!!");
        WRITEMSG
        goto ErrExit;
    }
#endif 


    set_zd_data("0210", "01"); 
    set_zd_double("1001", amt-drftAmt);
    set_zd_data("0660", "2");
    
    /****************************结转未确认的利息收入-------没有处理***************/
    
    /****************为B914准备数据**************/
    vtcp_log("%s.%d,sMo_discnt.pact_no=%s", __FILE__,__LINE__,sMo_discnt.pact_no);
    set_zd_data("0620",sMo_discnt.pact_no);
    set_zd_data("0670","2");   /*账户类型*/
    set_zd_data("0330",sMdm_ac_rel.ac_no);    /* 贷款帐号*               不明却*/
    set_zd_double("1002",drftAmt);
    /****************为D003准备数据***************/
    set_zd_data("0300",sCom_parm.val);
    set_zd_double("0400",amt);
    set_zd_data("0600",drftNb); 
    set_zd_data("0670","2");   /*账户类型*/
    set_zd_data("0680","2");
    /*set_zd_data("0890","299");*/
    set_zd_data("0900","111"); 
    /*****D003的准备****/
    set_zd_data("0200","1");/*不检查凭证*/
    set_zd_data("0240","000" );/*不检查凭证*/
    
    
    


GoodExit:
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
GoodExit2:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
    {
    	memcpy(g_pub_tx.reply,"B555",4);
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0; 
}
