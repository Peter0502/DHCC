/**************************************************************
* 文 件 名:  spB552.c 承兑通知(银承)
* 功能描述:  
*           本业务是在信贷系统审批后，将承兑汇票基本信息
*       填写到核心系统的承兑汇票基本信息登记簿中，农
*       信银系统得到审批结果，后会发送报文到平台，平
*       台整理后将平台报文转发到核心系统,本程序实现从
*       平台接收报文转发给核心的功能。
* 作    者:  chenchao
* 完成日期： 2010-4-6 9:37:33
*
* 修改记录：
*   日    期:
*   修 改 人:
*   修改内容:
insert into tx_def values ('4952','承兑通知(银承)','10000000000000000000000000000000000000000100000000000000010000000000000000000000111000000000000000000000000000000000000000000000','1','3','2','3',null)
insert into tx_flow_def  values ('4952',0,'B552','#=0001@1@#$')
insert into tx_flow_def  values ('4952',1,'B333','#=0000@2@#$')
insert into tx_flow_def  values ('4952',2,'D003','#=0000@3@#$')
insert into tx_flow_def  values ('4952',3,'A017','#$')
insert into tx_sub_def values ('B552','承兑通知(银承)','spB552','0','0')
*****************************************************************/
#define ERR_DEAL if( ret ) \
{\
    sprintf( acErrMsg, "sql error" ); \
    WRITEMSG \
    goto ErrExit; \
}
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "ecds_recv_reg_c.h"
#include "mo_bank_po_c.h"
#include "mdm_ac_rel_c.h"
#include "hv_orno_brno_c.h"

int spB552()
{
    int    ttlnum = 0;    /**读取的总条数**/
    int    cnt=0;
    int    i = 0;
    int    ret = 0,pub_ret=0,ins_flag=0;
    long   l_date=0;
    
    char   cTmpBuff[101];
    char   ctype[2];
    char   cPn_po[31];
    char   tmpstr[512];
    char   tmpmo[512];
    char   cPo_type[2];
    char   cDrfttyp[5];
    char   cmtname[20];
    char   procstate[10];
    char   cTrace_no[31];
    char   cOut_br_no[33];
    char   wherelist[1024];    /**查询条件**/

    double        dSign_amt=0.00;
    
    struct mo_bank_po_c       sMo_bank_po;
    struct mdm_ac_rel_c       sMdm_ac_rel;
    struct hv_orno_brno_c     sHv_orno_brno;
    struct ecds_recv_reg_c    sEcds_recv_reg,sEcdsrecvreg;
    struct mo_bank_acc_po_c   sMo_bank_acc_po;
    
    
    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    
    memset(tmpmo,0x00,sizeof(tmpmo));
    memset(ctype, 0x00, sizeof(ctype));
    memset(cmtname,0x00,sizeof(cmtname));
    memset(wherelist, 0, sizeof(wherelist));
    memset(procstate,0x00,sizeof(procstate));
    memset(cTrace_no,0x00,sizeof(cTrace_no));
    memset(cOut_br_no,0x00,sizeof(cOut_br_no));
    
    memset(&sMo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sEcdsrecvreg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sMo_bank_po,0x00,sizeof(struct mo_bank_po_c));
    
    vtcp_log("[%s][%d]", __FILE__,__LINE__);

    pub_base_sysinit();
    get_zd_long("0440",&l_date);                      /*日期*/
    get_zd_double("0410",&sMo_bank_acc_po.sign_amt);  /*票据金额*/
    get_zd_double("0420",&sMo_bank_acc_po.bail_amt);  /*保证金    */
    get_zd_double("0410",&dSign_amt);                 /*票据金额*/
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("101F",cTmpBuff);                    /*流水号*/
    zip_space(cTmpBuff);
    memcpy(cTrace_no , cTmpBuff , sizeof(cTrace_no)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0670",cTmpBuff);                     /*票据分类*/
    zip_space(cTmpBuff);
    memcpy(cPo_type , cTmpBuff , sizeof(cPo_type)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0180",cTmpBuff);                     /*票据分类*/
    zip_space(cTmpBuff);
    memcpy(cDrfttyp , cTmpBuff , sizeof(cDrfttyp)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("1243",cTmpBuff);                       /*票据号*/
    zip_space(cTmpBuff);
    memcpy(cPn_po , cTmpBuff , sizeof(cPn_po)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0810",cTmpBuff);  /*出票人账号*/
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.payer_ac_no , cTmpBuff , sizeof(sMo_bank_acc_po.payer_ac_no)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0580",cTmpBuff);                   /*出票人开户行*/
    zip_space(cTmpBuff);
    memcpy(cOut_br_no , cTmpBuff , sizeof(cOut_br_no)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0250",cTmpBuff);   /*收款人名称*/
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.payee_name , cTmpBuff , sizeof(sMo_bank_acc_po.payee_name)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    /**
    get_zd_data("0830",cTmpBuff);   *保证金账号*
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.bail_ac_no , cTmpBuff , sizeof(sMo_bank_acc_po.bail_ac_no)-1);
    ***/
    get_zd_data("0830",cTmpBuff);   
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.pact_no, cTmpBuff , sizeof(sMo_bank_acc_po.pact_no)-1);
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0930",cTmpBuff);
    zip_space(cTmpBuff);
    memcpy(ctype , cTmpBuff , sizeof(ctype)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data("0820",cTmpBuff);
    zip_space(cTmpBuff);
    memcpy(sMo_bank_acc_po.payee_ac_no , cTmpBuff , sizeof(sMo_bank_acc_po.payee_ac_no)-1);

    vtcp_log("[%s][%d]----%s,dSign_amt[%f]dSign_amt[%f]bail_amt[%s]",__FILE__,__LINE__,sMo_bank_acc_po.payee_ac_no,dSign_amt,sMo_bank_acc_po.bail_amt);
    vtcp_log("[%s][%d]   -------ctype=[%s]",__FILE__,__LINE__,ctype);
    vtcp_log("[%s][%d]",__FILE__,__LINE__);

    /**组成查询条件**/
    pub_base_strpack(cOut_br_no);

    if (cPo_type[0] != 'E')
    {
        sprintf(acErrMsg,"非电票退出cPo_type=[%s]",cPo_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "DP01");
        goto ErrExit;
    }
    ret = Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsrecvreg,"ecds_tx_date = %ld and ecds_trace_no ='%s'",l_date,cTrace_no);
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
        	pub_ret=sql_execute("update ecds_recv_reg  set bf_flag='2' where ecds_tx_date = %ld and ecds_trace_no ='%s' and bf_flag='1' ",l_date,cTrace_no);
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
    	sprintf(acErrMsg,"查询Ecds_recv_reg表错误!! [%s]",cOut_br_no);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT03" );
        goto ErrExit;
    }

    ret = Hv_orno_brno_Sel(g_pub_tx.reply,&sHv_orno_brno," or_br_no=%s ",cOut_br_no);
    if (ret)
    {
        sprintf(acErrMsg,"查询Hv_orno_brno表错误!! [%s]",cOut_br_no);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "S029" );
        goto ErrExit;
    }
    vtcp_log("[%s][%d]----%lf", __FILE__,__LINE__,dSign_amt);
    /**组成查询**
    sprintf(wherelist, "payer_ac_no='%s' and payer_br_no='%s' and  payee_ac_no = '%s' and  bail_ac_no='%s' and bail_amt=%lf  and acc_po_sts='1' ",    
        sMo_bank_acc_po.payer_ac_no, sHv_orno_brno.br_no, sMo_bank_acc_po.payee_ac_no, sMo_bank_acc_po.bail_ac_no, sMo_bank_acc_po.bail_amt);  
    ***/
    sprintf(wherelist, "pact_no='%s' and acc_po_sts='1' ",    
        sMo_bank_acc_po.pact_no);  
    vtcp_log("WHERE [%s]", wherelist); 

    ret = Mo_bank_acc_po_Dec_Sel(g_pub_tx.reply, wherelist);
    ERR_DEAL
    ttlnum = 0;

    while (1) 
    {
        ret = Mo_bank_acc_po_Fet_Sel(&sMo_bank_acc_po, g_pub_tx.reply);
        if (ret == 100 )
            break;
        vtcp_log("[%s][%d]", __FILE__,__LINE__);    
        if (ret)
        {
            sprintf(acErrMsg, "查询mo_bank_acc_po错!!ret=[%d]", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "AT06");
            goto ErrExit;
        }
        ttlnum++;
    }
    ret = Mo_bank_acc_po_Clo_Sel();
    ERR_DEAL

    if (!ttlnum) 
    {
        strcpy(g_pub_tx.reply, "S049");
        goto ErrExit;
    } 
    vtcp_log("[%s][%d]----%lf", __FILE__,__LINE__,dSign_amt);
    if(ins_flag == 0)
    {	
        /*插入表Ecds_recv_reg具体数值,状态字段*/
    	sEcds_recv_reg.ecds_tx_date  =l_date;
    	sEcds_recv_reg.trace_no   = 0;
    	sEcds_recv_reg.tx_date    = 0;
    	sEcds_recv_reg.sts[0]     ='1';
    	sEcds_recv_reg.check_flag[0]='1';
    	sEcds_recv_reg.bf_flag[0]='0';
    	sEcds_recv_reg.drftcls[0] =cPo_type[0];
    	sEcds_recv_reg.issedt     = sMo_bank_acc_po.req_date ;
    	sEcds_recv_reg.duedt      = sMo_bank_acc_po.matr_date ;
    	memcpy(sEcds_recv_reg.ecds_trace_no,  cTrace_no , sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
    	memcpy(sEcds_recv_reg.drfttyp      ,  cDrfttyp  , sizeof(sEcds_recv_reg.drfttyp)-1);
    	memcpy(sEcds_recv_reg.drftnb       ,  cPn_po    , sizeof(sEcds_recv_reg.drftnb)-1);
    	memcpy(sEcds_recv_reg.tag_name     , "CBS1000040"     , sizeof( sEcds_recv_reg.tag_name)-1);
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
    cnt = sql_max_int("mo_bank_po","pact_no_num","pact_no='%s'",sMo_bank_acc_po.pact_no);
    vtcp_log("[%s][%d] cnt=[%d] ",__FILE__,__LINE__,cnt);
    ret = Mdm_ac_rel_Sel (g_pub_tx.reply,&sMdm_ac_rel," ac_no='%s' ",sMo_bank_acc_po.payer_ac_no);
    if (ret)
    {
        sprintf(acErrMsg,"查询Mdm_ac_rel表错误!! [%s]",cOut_br_no);  
        WRITEMSG
        strcpy ( g_pub_tx.reply , "S029" );
        goto ErrExit;
    }
    /**********************************为B333数据准备的数据***********************************/
    vtcp_log("[%s][%d] 为B333准备数据[%s][%s][%f][%s]",__FILE__,__LINE__,sMo_bank_acc_po.pact_no,cPn_po,dSign_amt,sMo_bank_acc_po.payee_ac_no);
    set_zd_data("0330", sMo_bank_acc_po.pact_no);      /* 承兑协议编号 */
    set_zd_int("0500", (cnt+1));                       /* 编号 */
    set_zd_data("0580", cPn_po);                       /* 承兑汇票号码 */
    set_zd_double("100E",dSign_amt);                   /* 票面金额 */
    set_zd_data("0270",sMo_bank_acc_po.payee_name);    /* 收款人全称*/
    set_zd_data("0830",sMo_bank_acc_po.payee_ac_no);   /* 收款人账号*/
    set_zd_data("0810",sMo_bank_acc_po.payee_br_name); /* 收款人开户行行名*/
    set_zd_data("0930","E");                              /*电票纸票标志**/
    /**********************************为D003数据准备的数据***********************************/
    vtcp_log("[%s][%d] 为D099准备数据 [%s][%f][%s]",__FILE__,__LINE__,sMdm_ac_rel.id_type,sMo_bank_po.po_amt,sMo_bank_acc_po.payer_ac_no);
    set_zd_data("1023","111");                /*产品编号*/
    set_zd_data("102G", sMdm_ac_rel.id_type);
    set_zd_data("102A", sMdm_ac_rel.id_no );
    set_zd_long("102E",sMo_bank_acc_po.matr_date);
    /*set_zd_double("102F",1);                       手续费????????????????*/
    set_zd_double("102F",0);                       /*手续费改成由1--0 by 20150123 xmh*/
    set_zd_data("102J","01" );
    set_zd_data("102K","2" );
    set_zd_data("102Y", "银行汇兑");
    set_zd_data("102O","0" );                /*是否打折标志*/
    set_zd_data("1021",  sMo_bank_acc_po.payer_ac_no);
    set_zd_long("1022", sMdm_ac_rel.ac_seqn);
    vtcp_log("[%s][%d]----sMdm_ac_rel.ac_seqn=%d", __FILE__,__LINE__,sMdm_ac_rel.ac_seqn);
    set_zd_data("0910",sMo_bank_acc_po.tx_br_no );
    set_zd_data("0810", "银行汇兑");
    set_zd_int("0193",9001);
    /**********************************为A017数据准备的数据***********************************/
    vtcp_log("[%s][%d] 为A017准备数据sMo_bank_acc_po.payee_ac_no[%s],sMo_bank_po.po_no[%s]",__FILE__,__LINE__,sMo_bank_acc_po.payee_ac_no,cPn_po);
    set_zd_data("0650", "51110");/*手续费收入科目*/

    

GoodExit:
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
    {
    	memcpy(g_pub_tx.reply,"B552",4);
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
