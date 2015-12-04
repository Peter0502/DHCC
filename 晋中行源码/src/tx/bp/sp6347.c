/***************************************************************************/
/**文件名称: sp6347.c                                                      */
/**功能描述: 银行本票挂失                                                  */
/**备    注: 此交易没有取消交易,要收取手续费                               */
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "bplosthis_c.h"
sp6347()
{
    int     iRc=0;
    int     iSignday=0;
    int     iLostday=0;
    int     lLostcnt=0;
    char    sKinbr[6];
    char    sTel[5];
    char    sTemp[61];
    char    cPono[21];
    char    sAvbal[16];
    char    sTxamt[13];
    char    cFiller[61];
    double  dAvbal=0.0;
    double  dTxamt=0.0;
    struct  cashier_c      sCashier;     /***本票基本信息表***/
    struct  bplosthis_c    sBplosthis;   /***本票挂失信息表***/
    struct  TIS_GROUP
    {
        char inflag;          /*本票类型*/
        char signday[8];      /*出票日期*/
        char pono   [16];     /*本票号码*/  /* 20110422 [12->16] */
        char paybrno[12];     /*签发行行号*/
        char paybrname[60];   /*签发行行名*/
        char cashbrno[12];    /*兑付行行号*/
        char cashbrname[60];  /*兑付行行名*/
        char filler[60];      /*备      注*/
        char temp[749];
    }tis;
    /*初始化变量*/
    memset(&sCashier,0x00,sizeof(sCashier));
    memset(&sBplosthis,0x00,sizeof(sBplosthis));
    memset(&tis,0x00,sizeof(tis));
    
    memset(sTel    , 0 , sizeof(sTel));
    memset(sTemp   , 0 , sizeof(sTemp));
    memset(cPono   , 0 , sizeof(cPono));
    memset(cFiller , 0 , sizeof(cFiller));
    memset(sKinbr  , 0 , sizeof(sKinbr));
    
    pub_base_sysinit();
    /*从前台屏幕取值*/
    get_fd_data("1230",(char *)&tis);
    get_zd_data("0030",sKinbr);   /*交易行号*/
    get_zd_int("0050", &iLostday);/*挂失日期*/
    get_zd_data("0070",sTel);     /*操作员*/
    get_zd_data("0390",sAvbal);   /*票面金额*/
    get_zd_data("0400",sTxamt);   /*手续费*/
    str2dbl(sAvbal,strlen(sAvbal),0,&dAvbal);
    str2dbl(sTxamt,strlen(sTxamt),0,&dTxamt);
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    memcpy(cPono+(20-sizeof(tis.pono)),tis.pono,sizeof(tis.pono));
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    memcpy(sCashier.pono,cPono,sizeof(sCashier.pono)-1);
    vtcp_log("[%s][%d]sCashier.pono=[%s]\n",__FILE__,__LINE__,sCashier.pono);
    vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
    memcpy(sTemp,tis.signday,sizeof(tis.signday));
    pub_base_strpack_all(sTemp);
    vtcp_log("[%s][%d]sTemp=[%s]\n",__FILE__,__LINE__,sTemp);
    iSignday=atoi(sTemp);
    vtcp_log("[%s][%d]sSignday=[%d]\n",__FILE__,__LINE__,iSignday);
    /*****为什么呢???
    g_reply_int=Bplosthis_Sel(g_pub_tx.reply,&sBplosthis,"pono='%s' and lostday='%d' ",sCashier.pono,iLostday);
    if(g_reply_int==0)
    {
        sprintf(acErrMsg,"[%s]][%d]当日不可以重复挂失[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    ****/
    /***本行的,本身有记录,修改即可***/
    vtcp_log("[%s][%d]本行本票\n",__FILE__,__LINE__);
    vtcp_log("[%s][%d]sCashier.pono=[%s]\n",__FILE__,__LINE__,sCashier.pono);
    vtcp_log("[%s][%d]iSignday=[%d]\n",__FILE__,__LINE__,iSignday);
    g_reply_int=Cashier_Sel(g_pub_tx.reply,&sCashier,"pono='%s' and signday='%d' ",sCashier.pono,iSignday);
    if(g_reply_int!=0)
    {
        sprintf(acErrMsg,"[%s]][%d]无此本票信息[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(sCashier.stat[0]!='1')    /*'1'签发状态*/
    {
        vtcp_log("[%s][%d]本票状态错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"本票状态错误");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(fabs(sCashier.avbal-dAvbal)>0.0001)
    {
        vtcp_log("[%s][%d]dAvbal=[%f]\n",__FILE__,__LINE__,dAvbal);
        vtcp_log("[%s][%d]sCashier.avbal=[%f]\n",__FILE__,__LINE__,sCashier.avbal);
        vtcp_log("[%s][%d]金额不一致\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"金额不一致");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /****2008年04年29日，梁皓然注销这一段
    if(memcmp(sCashier.brno,sKinbr,sizeof(sKinbr)-1)!=0)
    {
        vtcp_log("[%s][%d]非本行签发,不能挂失\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"请到签发行挂失");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    ****/
    memset(sTemp,0x00,sizeof(sTemp));
    memcpy(sTemp,tis.paybrname,sizeof(sTemp)-1);
    pub_base_strpack_all(sTemp);
    if(memcmp(sCashier.paybrname,sTemp,strlen(sTemp)))
    {
        vtcp_log("[%s][%d]签发行行名不符\n",__FILE__,__LINE__);
        vtcp_log("[%s][%d]sTemp[%s]\n",__FILE__,__LINE__,sTemp);
        vtcp_log("[%s][%d]sCashier.paybrname[%s]\n",__FILE__,__LINE__,sCashier.paybrname);
        set_zd_data(DC_GET_MSG,"签发行行名不符");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(memcmp(sCashier.cashbrno,tis.cashbrno,sizeof(sCashier.cashbrno)-1))
    {
        vtcp_log("[%s][%d]兑付行行号不符\n",__FILE__,__LINE__);
        vtcp_log("[%s][%d]sCashier.cashbrno=[%s]\n",__FILE__,__LINE__,sCashier.cashbrno);
        vtcp_log("[%s][%d]tis.cashbrno=[%s]\n",__FILE__,__LINE__,tis.cashbrno);
        set_zd_data(DC_GET_MSG,"兑付行行号不符");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    memset(sTemp,0x00,sizeof(sTemp));
    memcpy(sTemp,tis.cashbrname,sizeof(sTemp)-1);
    pub_base_strpack_all(sTemp);
    if(memcmp(sCashier.cashbrname,sTemp,strlen(sTemp)))
    {
        vtcp_log("[%s][%d]兑付行行名不符\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"兑付行行名不符");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]要产生一条挂失记录\n",__FILE__,__LINE__);
    /*产生一条挂失信息*/
    memcpy(sBplosthis.pono,sCashier.pono,sizeof(sBplosthis.pono)-1);
    memcpy(sBplosthis.paybrno,sCashier.paybrno,sizeof(sBplosthis.paybrno)-1);
    memcpy(sBplosthis.paybrname,sCashier.paybrname,sizeof(sBplosthis.paybrname)-1);
    memcpy(sBplosthis.cashbrno,sCashier.cashbrno,sizeof(sBplosthis.cashbrno)-1);
    memcpy(sBplosthis.cashbrname,sCashier.cashbrname,sizeof(sBplosthis.cashbrname)-1);
    memcpy(sBplosthis.lostbrno,sKinbr,sizeof(sKinbr)-1);
    memcpy(sBplosthis.bptype,sCashier.bptype,sizeof(sKinbr)-1);

    memcpy(cFiller,tis.filler,sizeof(cFiller)-1);
    pub_base_strpack(cFiller);
    memcpy(sBplosthis.filler,cFiller,sizeof(sBplosthis.filler)-1);
    
    sBplosthis.lostday=iLostday;        /*挂失日期*/
    sBplosthis.signday=iSignday;        /*签发日期*/
    sBplosthis.inflag[0]=tis.inflag;

    sBplosthis.avbal=dAvbal;
    sBplosthis.stat[0]='0';            /*0挂失1解挂失*/
    
    lLostcnt=sql_count("bplosthis","pono='%s' and lostday='%d' ",sCashier.pono,iLostday);
    if(lLostcnt<0){
       vtcp_log("[%s][%d]未找到符合条件的记录![%d]\n",__FILE__,__LINE__,lLostcnt);
       strcpy(g_pub_tx.reply,"S049");
       goto ErrExit;
    }
    sBplosthis.lostcnt=lLostcnt+1;              /*挂失次数*/
    
    vtcp_log("[%s][%d]挂失次数[%d]\n",__FILE__,__LINE__,sBplosthis.lostcnt);
    memcpy(sBplosthis.losttlrno,sTel,sizeof(sBplosthis.losttlrno)-1);
    g_reply_int=Bplosthis_Ins(sBplosthis,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]插入挂失登记簿错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"插入挂失登记簿错误");
        Bplosthis_Debug(&sBplosthis);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*更新本票信息*/
    vtcp_log("[%s][%d]要更新本票信息\n",__FILE__,__LINE__);
    if((g_reply_int=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%d' ",sCashier.pono,iSignday))!=0){
        vtcp_log("[%s][%d]本票不存在\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"本票不存在");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if((g_reply_int=Cashier_Fet_Upd(&sCashier,g_pub_tx.reply))!=0)
    {
        vtcp_log("[%s][%d]锁定此本票号错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"锁定此本票号错误");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    sCashier.lactcls[0]=sCashier.stat[0];
    sCashier.stat[0]='4';        /*挂失*/
    sCashier.lostcnt++;
    iRc=Cashier_Upd_Upd(sCashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]更新本票错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"更新本票错误");
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    vtcp_log("[%s][%d]更新本票结束\n",__FILE__,__LINE__);
    /**登记交易流水**/
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]登记交易流水出错[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"登记交易流水出错",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    vtcp_log("[%s][%d]程序处理完毕\n",__FILE__,__LINE__);
OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
