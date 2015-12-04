/***************************************************************************/
/**文件名称: sp6348.c                                                      */
/**功能描述: 银行本票解挂失                                                */
/**备    注: 此交易没有取消交易                                            */
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "bplosthis_c.h"
sp6348()
{
    int     iRc=0;
    long    lLostday=0;
    
    char    sTel [ 5];
    char    cTemp[17];  /* 20110422 [13->17] */
    char    cPono[21];
    char    sKinbr[6];
    
    struct  cashier_c      sCashier;     /***本票基本信息表***/
    struct  bplosthis_c    sBplosthis;   /***本票挂失信息表***/

    /****初始化变量****/
    memset(sTel  , 0 , sizeof(sTel));
    memset(cTemp , 0 , sizeof(cTemp));
    memset(cPono , 0 , sizeof(cPono));
    memset(sKinbr, 0 , sizeof(sKinbr));
    memset(&sCashier  , 0 , sizeof(sCashier));
    memset(&sBplosthis, 0 , sizeof(sBplosthis));
    
    pub_base_sysinit();
    
    /*从前台屏幕取值*/
    get_zd_long("0440",&lLostday);  /*挂失日期*/
    get_fd_data("0320",cTemp);      /*本票号码*/
    get_zd_data("0030",sKinbr);     /*交易行号*/
    get_zd_data("0070",sTel);       /*操 作 员*/
    
    vtcp_log("[%s][%d]lLostday=[%ld]\n",__FILE__,__LINE__,lLostday);
    vtcp_log("[%s][%d]   cTemp=[%s]\n",__FILE__,__LINE__,cTemp);
    vtcp_log("[%s][%d]  sKinbr=[%s]\n",__FILE__,__LINE__,sKinbr);
    vtcp_log("[%s][%d]    sTel=[%s]\n",__FILE__,__LINE__,sTel);
    
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    memcpy(cPono+4,cTemp,sizeof(cTemp)-1);   /* 20110422 [8->4] */
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    /**修改挂失登记簿信息**/
    if((g_reply_int=Bplosthis_Dec_Upd(g_pub_tx.reply,"pono='%s' and lostday='%d' and stat='0' ",cPono,lLostday))!=0)
    {
        sprintf(acErrMsg,"[%s][%d]挂失登记簿不存在[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if((g_reply_int=Bplosthis_Fet_Upd(&sBplosthis,g_pub_tx.reply))!=0)
    {
        sprintf(acErrMsg,"[%s][%d]挂失登记簿不存在[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(sBplosthis.stat[0]!='0')
    {
        sprintf(acErrMsg,"[%s][%d]本票状态不是挂失,不能解挂\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]sKinbr=[%s]\n",__FILE__,__LINE__,sKinbr);
    memcpy(sBplosthis.unlostbrno,sKinbr,sizeof(sKinbr)-1);
    sBplosthis.unlostday=g_pub_tx.tx_date;
    sBplosthis.stat[0]='1';                            /*解挂失*/
    memcpy(sBplosthis.unlosttlrno,sTel,sizeof(sTel)-1);/*解挂操作员*/
    iRc=Bplosthis_Upd_Upd(sBplosthis,g_pub_tx.reply);
    if(iRc!=0)
    {
        sprintf(acErrMsg,"[%s][%d]更新本票挂失登记簿错误[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Bplosthis_Clo_Upd();
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    vtcp_log("[%s][%d]sBplosthis.inflag=[%c]\n",__FILE__,__LINE__,sBplosthis.inflag[0]);
    vtcp_log("[%s][%d]sBplosthis.signday=[%d]\n",__FILE__,__LINE__,sBplosthis.signday);
    /*更新本票信息*/
    if((g_reply_int=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%d' ",cPono,sBplosthis.signday))!=0)
    {
        sprintf(acErrMsg,"[%s][%d]无此本票信息[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if((g_reply_int=Cashier_Fet_Upd(&sCashier,g_pub_tx.reply))!=0)
    {
        sprintf(acErrMsg,"[%s][%d]本票信息不存在[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(sCashier.stat[0]!='4')
    {    /***'4'本票挂失***/
        sprintf(acErrMsg,"[%s][%d]原本票状态错[%d]\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    sCashier.lactcls[0]=sCashier.stat[0];
    sCashier.stat[0]='1';  /***解挂失***/
    sCashier.lostcnt--;    /***挂失次数***/
    /***本行本票,改状态***/
    iRc=Cashier_Upd_Upd(sCashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        sprintf(acErrMsg,"[%s][%d]更新本票错误[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    vtcp_log("[%s][%d]解挂完成!!\n",__FILE__,__LINE__);
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
