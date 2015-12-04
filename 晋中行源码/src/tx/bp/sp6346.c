/***************************************************************************/
/**文件名称: sp6346.c                                                      */
/**功能描述: 银行本票退回                                                  */
/**函数功能: 实现各个支行退回本票.                       		           */
/**修    改：xyy      增加处理账务功能                                     */        
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_parm_c.h"
sp6346()
{
    int     iRc=0,ret=0;
    char    cTel[5];
    char    cKinbr[6];
    char    cBrno[6];
    char    cThflag[2];
    char    cPono[21];
    char    cTemp[17];    /* 20110422 [13->17] */
    char    cContent[61];
    double  dTxamt=0.0;
    long    lSignday=0;
    struct  com_branch_c   sCom_branch;  /****机构表***/
    struct  com_parm_c     com_parm;     /****公共参数表*****/
    struct  in_mst_c       in_mst;       /****内部台帐主文件******/
    struct  cashier_c      sCashier;     /***本票基本信息表*******/
    struct  mdm_ac_rel_c   mdm_ac_rel;   /****介质与账户关系表****/
    /****初始化变量****/
    memset(cTel        , 0 , sizeof(cTel));
    memset(cPono       , 0 , sizeof(cPono));
    memset(cTemp       , 0 , sizeof(cTemp));
    memset(cBrno       , 0 , sizeof(cBrno));
    memset(cKinbr      , 0 , sizeof(cKinbr));
    memset(cThflag     , 0 , sizeof(cThflag));
    memset(cContent    , 0 , sizeof(cContent));
    memset(&com_parm    ,0 , sizeof(com_parm));
    memset(&in_mst      ,0 , sizeof(in_mst));
    memset(&mdm_ac_rel  ,0 , sizeof(mdm_ac_rel));
    memset(&sCashier   , 0 , sizeof(sCashier));
    memset(&sCom_branch, 0 , sizeof(sCom_branch));
    pub_base_sysinit();
    /*从前台屏幕取值*/
    get_zd_long("0450",&lSignday);  /*签发日期*/
    get_zd_data("0070",cTel);      /*签发日期*/
    get_fd_data("0320",cTemp);     /*本票号码*/
    get_zd_data("0030",cKinbr);    /*交易行号*/
    get_zd_data("0670",cThflag);   /*退回类型*/
    get_zd_data("0250",cContent);  /*摘    要*/
    vtcp_log("[%s][%d] cTel=[%s]\n",__FILE__,__LINE__,cTel);
    vtcp_log("[%s][%d] cTemp=[%s]\n",__FILE__,__LINE__,cTemp);
    vtcp_log("[%s][%d]cKinbr=[%s]\n",__FILE__,__LINE__,cKinbr);
    vtcp_log("[%s][%d]lSignday=[%d]\n",__FILE__,__LINE__,lSignday);
    vtcp_log("[%s][%d]cContent=[%s]\n",__FILE__,__LINE__,cContent);
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    memcpy(cPono+4,cTemp,sizeof(cTemp)-1);     /* 20110422 [8->4] */
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    /*检查机构是否存在*/
    iRc=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s' ",cKinbr);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]无此交易机构\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"无此交易机构");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*修改本票状态*/
    g_reply_int=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%d' ",cPono,lSignday);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]本票不存在\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]本票不存在[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    g_reply_int=Cashier_Fet_Upd((char*)&sCashier,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]锁定此本票号错误\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]锁定此本票号错误[%d]\n",__FILE__,__LINE__,g_reply_int);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(memcmp(sCashier.brno,cKinbr,sizeof(cKinbr)-1)!=0)
    {
        vtcp_log("[%s][%d]请到签发机构做未用退回\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]请到签发机构做未用退回\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(sCashier.stat[0]!='1')
    {
        vtcp_log("[%s][%d]本票不是签发状态\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]本票不是签发状态\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    sCashier.checkflag[0]='0';
    pub_base_strpack(cContent);
    sCashier.stat[0]='6';/*本票退回状态*/
    sCashier.lactcls[0]=sCashier.stat[0];
    memcpy(sCashier.filler,cContent,strlen(cContent));
    memcpy(sCashier.jqtlrno,cTel,sizeof(sCashier.jqtlrno)-1);
    memcpy(cBrno,sCashier.brno,sizeof(cBrno)-1);/**签发机构**/
    iRc=Cashier_Upd_Upd(sCashier,g_pub_tx.reply);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]更新本票错误\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"[%s][%d]更新本票错误[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    /*要给spA016子交易赋值*/
    vtcp_log("[%s][%d]更新本票记录完毕\n",__FILE__,__LINE__);
    /**登记交易流水**/
    memcpy(g_pub_tx.beg_note_no,cTemp,16);/***NEWBP3***/  /* 20110425 [12->16] */
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]登记交易流水出错[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"登记交易流水出错\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /**检查签发行的兑付帐号**/
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=88",cBrno);
    if(ret)
    {
        vtcp_log("[%s][%d]承兑付款临时账户错误[%d]\n",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"[%s][%d]承兑付款临时帐户错误!ret=[%d]\n",__FILE__,__LINE__,ret);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        WRITEMSG
        goto ErrExit;
    }
    /**检查本票签发户是否存在**/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
    if(ret)
    {
        vtcp_log("[%s][%d]检查本票签发户失败[%d]\n",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"[%s][%d]检查本票签发户失败!ret=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"8888");
        WRITEMSG
        goto ErrExit;
    }
    ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
    if(ret)
    {
        vtcp_log("[%s][%d]本票签发户错误[%d]\n",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"[%s][%d]本票签发户错误!ret=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"8888");
        WRITEMSG
        goto ErrExit;
    }
    if(in_mst.sts[0]!='1'&&in_mst.sts[0]!='2')
    {
        vtcp_log("[%s][%d]帐号状态非正常\n",__FILE__,__LINE__);
        sprintf(acErrMsg,"账号状态非正常状态![%ld,%s]",in_mst.ac_id,in_mst.sts);
        strcpy(g_pub_tx.reply,"8888");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
    }
    set_zd_double("0390",sCashier.avbal);
    set_zd_double("0400",sCashier.avbal);
    set_zd_data("0210","01");
    set_zd_data("0880","未用退回");
    set_zd_data("0300",com_parm.val);
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
