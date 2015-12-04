/***************************************************************************/
/*文件名称: sp6341.c                                                       */
/*功能描述: 签发本票信息的录入                                             */
/*作    者: xyy    2008-02-25                                              */  
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "com_item_c.h"
#include "hv_uniontab_c.h"
#include "lv_define.h"
#include "mdm_ac_rel_c.h"
extern int iLvGetReceOpnbr(const char * tx_br_no,const char * cash_ac_no,char * opn_br_no);
int sp6341()
{
    int     iRc=0;
    char    cPayactno[33];
    char    cOrbrno[13];
    long    lSignday=0;
    char    cKinbr[6];
    char    cTel[5];
    char    cPono[21];
    char    cQsno[13];
    char    cPaybrno[13];
    char    cPaybrname[61];
    char    cOpbrno[6];
    char    cTxamt[17];
    char    cInflag[2];
   
    double  dTxamt=0.0;
    struct  mdm_ac_rel_c   mdm_ac_rel;
    
    struct  cashier_c      Cashier;      /***本票基本信息表***/
    struct  hv_uniontab_c  Hv_uniontab1; /***行名行号信息表***/
    struct  hv_uniontab_c  Hv_uniontab2; /***行名行号信息表***/
    struct  hv_uniontab_c  Hv_uniontab3; /***行名行号信息表***/
    struct  com_branch_c   Com_branch;   /***机构表***/
    struct TIS_GROUP{
        char bptype    [1 ];    /* 本票类别 0-可转让本票1-转帐本票2-现金本票 */
        char avbaltype [1 ];    /* 定额标志 0-定额本票1-不定额本票 */
        char pono      [16];    /* 本票号码 */                    /* 20110422 [12 -> 16] */
        char signtype  [1 ];    /* 签发方式 0-现金,1-对公帐号,2-系统内转 */
        char payactno  [32];    /* 付款人帐号 */
        char payname   [60];    /* 付款人名字 */
        char cashactno [32];    /* 收款人帐号 */
        char cashname  [60];    /* 收款人名字 */
        char cashbrno  [12];    /* 收款人行号(兑付行) */
        char cashbrname[60];    /* 兑付行行名 */
        char reason    [60];    /* 摘要 */
        char temp      [614];
    }tis;
    /****初始化变量****/
    memset(&tis         , 0 , sizeof(tis));
    memset(cTel         , 0 , sizeof(cTel));
    memset(cPono        , 0 , sizeof(cPono));
    memset(cQsno        , 0 , sizeof(cQsno));
    memset(cTxamt       , 0 , sizeof(cTxamt));
    memset(cKinbr       , 0 , sizeof(cKinbr));
    
    memset(cOrbrno      , 0 , sizeof(cOrbrno));
    memset(cInflag      , 0 , sizeof(cInflag));
    memset(cOpbrno      , 0 , sizeof(cOpbrno));
    memset(cPaybrno     , 0 , sizeof(cPaybrno));
    memset(cPayactno    , 0 , sizeof(cPayactno));
    memset(cPaybrname   , 0 , sizeof(cPaybrname));
    memset(&Cashier     , 0 , sizeof(Cashier));
    memset(&Com_branch  , 0 , sizeof(Com_branch));
    memset(&Hv_uniontab1, 0 , sizeof(Hv_uniontab1));
    memset(&Hv_uniontab2, 0 , sizeof(Hv_uniontab2));
    memset(&Hv_uniontab3, 0 , sizeof(Hv_uniontab3));
    memset(&mdm_ac_rel  , 0 , sizeof(mdm_ac_rel));

    pub_base_sysinit();
    vtcp_log("[%s][%d]同志们sp6341开始了\n",__FILE__,__LINE__);
    /*从前台屏幕取值*/
    get_fd_data("1230",(char *)&tis);
    vtcp_log("[%s][%d] tis.bptype    =[%.1s] \n",__FILE__,__LINE__,tis.bptype     );
    vtcp_log("[%s][%d] tis.avbaltype =[%.1s] \n",__FILE__,__LINE__,tis.avbaltype  );
    vtcp_log("[%s][%d] tis.pono      =[%.16s]\n",__FILE__,__LINE__,tis.pono       );
    vtcp_log("[%s][%d] tis.signtype  =[%.1s] \n",__FILE__,__LINE__,tis.signtype   );
    vtcp_log("[%s][%d] tis.payactno  =[%.32s]\n",__FILE__,__LINE__,tis.payactno   );
    vtcp_log("[%s][%d] tis.payname   =[%.60s]\n",__FILE__,__LINE__,tis.payname    );
    vtcp_log("[%s][%d] tis.cashactno =[%.32s]\n",__FILE__,__LINE__,tis.cashactno  );
    vtcp_log("[%s][%d] tis.cashname  =[%.60s]\n",__FILE__,__LINE__,tis.cashname   );
    vtcp_log("[%s][%d] tis.cashbrno  =[%.12s]\n",__FILE__,__LINE__,tis.cashbrno   );
    vtcp_log("[%s][%d] tis.cashbrname=[%.60s]\n",__FILE__,__LINE__,tis.cashbrname );
    vtcp_log("[%s][%d] tis.reason    =[%.60s]\n",__FILE__,__LINE__,tis.reason     );
    get_zd_data("0030",cKinbr);        /*交易行号*/
    get_zd_long("0050",&lSignday);     /*签发日期*/
    vtcp_log("[%s][%d]lSignday=[%d]\n",__FILE__,__LINE__,lSignday);
    get_zd_data("0070",cTel);          /*签发日期*/
    vtcp_log("[%s][%d]cTel=[%s]\n",__FILE__,__LINE__,cTel);
    get_zd_data("0390",cTxamt);       /*出票金额*/
    vtcp_log("[%s][%d]cTxamt=[%s]\n",__FILE__,__LINE__,cTxamt);
    str2dbl(cTxamt,strlen(cTxamt),0,&dTxamt);
    vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
    /***判断是否是清算中心***/
    pub_base_strpack(cKinbr);
    vtcp_log("[%s][%d]cKinbr=[%s]\n",__FILE__,__LINE__,cKinbr);
    if(memcmp(cKinbr,QS_BR_NO,5)==0)
    {
        vtcp_log("%s,%d 清算中心不可以签发本票",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"清算中心不可以签发本票",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***判断交易行是否存在***/
    iRc=Com_branch_Sel(g_pub_tx.reply,&Com_branch,"br_no='%s' ",cKinbr);
    if(iRc!=0)
    {
        vtcp_log("%s,%d无此交易机构[%d]",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"[%s][%d]无此交易机构[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***根据交易行号,得到参与者行号***/
    g_reply_int=GetOr_br_noByBr_no(cKinbr,cOrbrno);
    vtcp_log("[%s][%d]cOrbrno=[%s]\n",__FILE__,__LINE__,cOrbrno);
    if(g_reply_int)
    {
        sprintf(acErrMsg,"查找支付系统行号错误",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"P007",4);
        set_zd_data(DC_GET_MSG,acErrMsg);
        vtcp_log("%s,%d 查找支付系统行号错误",__FILE__,__LINE__);
        goto ErrExit;
    }
    /***检查兑付行***/
    vtcp_log("[%s][%d]检查兑付行为:[%.12s]\n",__FILE__,__LINE__,tis.cashbrno);
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab1,"or_br_no='%.12s' ",tis.cashbrno);
    if(iRc!=0)
    {
        vtcp_log("%s,%d查询发起行参与者行号错误[%s]",__FILE__,__LINE__,tis.cashbrno);
        sprintf(acErrMsg,"[%s][%d]查询发起行参与者行号错误",__FILE__,__LINE__);
       /** set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit; **/
    }
    /**得到兑付行的清算行号**/
    memcpy(cQsno ,Hv_uniontab1.qs_no,sizeof(Hv_uniontab1.qs_no)-1);
    vtcp_log("[%s][%d]兑付清算行:[%s]\n",__FILE__,__LINE__,cQsno);
    /***查询签发行的基本信息***/
    pub_base_strpack(cOrbrno);
    vtcp_log("[%s][%d]签发行:[%s]\n",__FILE__,__LINE__,cOrbrno);
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab2,"or_br_no='%s' ",cOrbrno);
    if(iRc!=0)
    {
        vtcp_log("%s,%d 查询发起行参与者行号错误[%s]",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"[%s][%d]查询发起行参与者行号错误",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /**判断兑付行和签发行是否为同一清算中心**/
    if(memcmp(cQsno,Hv_uniontab2.qs_no,sizeof(Hv_uniontab2.qs_no)-1)==0)
    {
        cInflag[0]='0';/*行内*/
    }
    else
    {
        cInflag[0]='1';/*行外*/
    }
    vtcp_log("[%s][%d]行内标志[%c]\n",__FILE__,__LINE__,cInflag[0]);
    /**本票号码前部8个零**/
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);
    memcpy(cPono+4,tis.pono,sizeof(tis.pono));                        /* 20110422 [8位零->4位零] */
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);
    /**检查是否有这个本票存在**/
    iRc=Cashier_Sel(g_pub_tx.reply,&Cashier,"pono='%s' and signday='%ld' ",\
                    cPono,lSignday);
    if(iRc==0)
    {
        vtcp_log("%s,%d此本票号已经用过[%d]",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"[%s][%d]此本票号已经用过[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /**初始化这个结构体,下边还要重新赋值**/
    memset(&Cashier,0, sizeof(Cashier));
    /**检查金额是否为零**/
    vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
    if(dTxamt<=0.0)
    {
        vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
        sprintf(acErrMsg,"[%s][%d]金额不可以为零\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /**得到付款帐号的开户机构**/
    memcpy(cPayactno,tis.payactno,sizeof(cPayactno)-1);
    vtcp_log("[%s][%d]cPayactno=[%s]\n",__FILE__,__LINE__,cPayactno);
    pub_base_strpack(cPayactno);
    if(strlen(cPayactno)>=7)
    {
        iRc=iLvGetReceOpnbr(cKinbr,cPayactno,cOpbrno);
        if(iRc!=0)
        {
            vtcp_log("[%s][%d]得到开户机构错误\n",__FILE__,__LINE__);
            sprintf(acErrMsg,"[%s][%d]得到开户机构错误\n",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            strcpy(g_pub_tx.reply,"8888");
            goto ErrExit;
        }
        vtcp_log("[%s][%d]付款帐号开户行为:[%s]\n",__FILE__,__LINE__,cOpbrno);
        /***根据交易行号,得到参与者行号***/
        g_reply_int=GetOr_br_noByBr_no(cOpbrno,cPaybrno);
        vtcp_log("[%s][%d]cPaybrno=[%s]\n",__FILE__,__LINE__,cPaybrno);
        if(g_reply_int)
        {
            sprintf(acErrMsg,"查找支付系统行号错误",__FILE__,__LINE__);
            strncpy(g_pub_tx.reply,"P007",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            vtcp_log("%s,%d 查找支付系统行号错误",__FILE__,__LINE__);
            goto ErrExit;
        }
        vtcp_log("[%s][%d]付款开户行为:[%s]\n",__FILE__,__LINE__,cPaybrno);
        pub_base_strpack(cPaybrno);
        iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab3,"or_br_no='%s' ",cPaybrno);
        if(iRc!=0)
        {
            vtcp_log("%s,%d 查询发起行参与者行号错误[%s]",__FILE__,__LINE__,cOrbrno);
            sprintf(acErrMsg,"[%s][%d]查询发起行参与者行号错误",__FILE__,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            strcpy(g_pub_tx.reply,"8888");
            goto ErrExit;
        }
        memcpy(cPaybrname,Hv_uniontab3.br_name_f,sizeof(cPaybrname)-1);   /*机构全称*/
    }
    else
    {
        memcpy(cPaybrno,cOrbrno,sizeof(cPaybrno)-1);
        memcpy(cOpbrno,cKinbr,sizeof(cOpbrno)-1);
        memcpy(cPaybrname,Hv_uniontab2.br_name_f,sizeof(cPaybrname)-1);   /*机构全称*/
    }
    vtcp_log("[%s][%d]开始生成本票记录[%s]\n",__FILE__,__LINE__,cKinbr);
    /***开始生成本票记录***/
    memcpy(Cashier.pono,cPono,sizeof(cPono)-1);
    Cashier.txday=lSignday;
    memcpy(Cashier.orbrno,cOrbrno,sizeof(Cashier.orbrno)-1);
    memcpy(Cashier.payqsactno,Hv_uniontab2.qs_no,sizeof(Cashier.payqsactno)-1);
    Cashier.avbaltype[0]=tis.avbaltype[0];
    Cashier.bptype[0]=tis.bptype[0]; 
    Cashier.ccflag[0]=tis.signtype[0];
    vtcp_log("[%s][%d]本记录的票据号码:[%s]\n",__FILE__,__LINE__,Cashier.pono);
    memcpy(Cashier.brno,cKinbr,sizeof(cKinbr)-1);

    memcpy(Cashier.payactno,cPayactno,sizeof(Cashier.payactno)-1);
    memcpy(Cashier.payname,tis.payname,sizeof(Cashier.payname)-1);
    vtcp_log("[%s][%d]付款人帐户:[%s]\n",__FILE__,__LINE__,Cashier.payactno);
    memcpy(Cashier.cour,"001",sizeof(Cashier.cour)-1);/*币种*/
    Cashier.avbal=dTxamt;
    memcpy(Cashier.cashactno,tis.cashactno,sizeof(Cashier.cashactno)-1);
    memcpy(Cashier.cashname,tis.cashname,sizeof(Cashier.cashname)-1);
    memcpy(Cashier.cashbrno,tis.cashbrno,sizeof(Cashier.cashbrno)-1);
    memcpy(Cashier.cashbrname,tis.cashbrname,sizeof(Cashier.cashbrname)-1);
    vtcp_log("[%s][%d]收款人帐户[%s]\n",__FILE__,__LINE__,Cashier.cashactno);
    memcpy(Cashier.paybrno,cPaybrno,sizeof(Cashier.paybrno)-1);
    memcpy(Cashier.paybrname,cPaybrname,sizeof(Cashier.paybrname)-1);   /*机构全称*/
    memcpy(Cashier.reason,tis.reason,sizeof(Cashier.reason)-1);/*附言*/
    Cashier.stat[0]='0';               /***登记***/
    Cashier.inflag[0]=cInflag[0];      /***行内***/
    vtcp_log("[%s][%d]行内外标示:[%c]\n",__FILE__,__LINE__,Cashier.inflag[0]);
    Cashier.addpassflg[0]='0';         /***加押未处理***/
    Cashier.signday=lSignday;
    vtcp_log("[%s][%d]准备计算本票到期日\n",__FILE__,__LINE__);
    pub_base_deadlineM(lSignday,2,&Cashier.endday);
    vtcp_log("[%s][%d]本票到期日[%ld]\n",__FILE__,__LINE__,Cashier.endday);
    memcpy(Cashier.optlrno,cTel,sizeof(cTel)-1);
    Cashier.lostcnt=0 ;                /***挂失次数***/
    vtcp_log("[%s][%d]签发日期:[%ld]\n",__FILE__,__LINE__,Cashier.signday);
    g_reply_int=Cashier_Ins(Cashier,g_pub_tx.reply);
    if(g_reply_int)
    {
    	Cashier_Debug(&Cashier);
        vtcp_log("[%s][%d]插入本票信息登记薄出错[%d]",__FILE__,__LINE__,g_reply_int);
        sprintf(acErrMsg,"插入本票信息登记薄出错");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]本票记录生成\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d]本票号码pono=[%s]\n",__FILE__,__LINE__,tis.pono);
    memcpy(g_pub_tx.beg_note_no,tis.pono,16);                     /* 20110422 [12位 -> 16位] */
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]登记交易流水出错[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"登记交易流水出错",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]登记交易流水完成",__FILE__,__LINE__);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
    {
        memcpy(g_pub_tx.reply,"T063",4);
    }
    sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
