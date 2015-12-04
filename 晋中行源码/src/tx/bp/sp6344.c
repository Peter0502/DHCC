/***************************************************************************/
/*文件名称: sp6344.c                                                       */
/*功能描述: 实现各个支行兑付本票                                           */
/*作    者: xyy    2008-2-25                                               */    
/***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "com_item_c.h"
#include "hv_uniontab_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_parm_c.h"
#include "hv_define.h"
int sp6344()
{
    int     iRc=0;
    int     ret=0;
    long    lSignday=0;
    long    lEndday =0;
    char    cKinbr[6];
    char    cPayactno[33];
    char    cPayname[61];
    char    cCashactno[33];
    char    cCashname[61];
    char    cOcashname[61];
    char    cTel[5];
    char    cPono[21];
    char    cMiya[21];
    char    cBrno[6];
    char    cOrbrno[13];
    char    cTemp[17];  /* 20110422 [13->17] */
    char    cTxamt[17];
    double  dTxamt=0.0;
    struct  com_parm_c     com_parm;
    struct  mdm_ac_rel_c   mdm_ac_rel;
    struct  in_mst_c       in_mst;
    struct  cashier_c      Cashier;      /***本票基本信息表***/
    struct  com_branch_c   Com_branch;   /***机构表***/
    struct  hv_uniontab_c  Hv_uniontab;
    struct TIS_GROUP{
        char bptype    [1 ];    /* 本票类别 0-可转让本票1-转帐本票2-现金本票 */
        char avbaltype [1 ];    /* 定额标志 0-定额本票1-不定额本票 */
        char pono      [16];    /* 本票号码 */  /* 20110422 [12->16] */
        char signtype  [1 ];    /* 签发方式 0-现金,1-对公帐号,2-系统内转 */
        char reqno     [10];    /* 申请书号码 */
        char payactno  [32];    /* 付款人帐号 */
        char payname   [60];    /* 付款人名字 */
        char cashactno [32];    /* 收款人帐号 */
        char cashname  [60];    /* 收款人名字 */
        char reason    [60];    /* 摘要 */
        char mac       [20];
        char temp      [656];
    }tis;
    /****初始化变量****/
    memset(&com_parm    , 0 , sizeof(com_parm));
    memset(&in_mst      , 0 , sizeof(in_mst));
    memset(&tis         , 0 , sizeof(tis));
    memset(&Hv_uniontab , 0 , sizeof(Hv_uniontab));
    memset(&mdm_ac_rel  , 0 , sizeof(mdm_ac_rel));
    memset(&Cashier     , 0 , sizeof(Cashier));
    memset(&Com_branch  , 0 , sizeof(Com_branch));
    memset(cTemp        , 0 , sizeof(cTemp));
    memset(cPayactno    , 0 , sizeof(cPayactno));
    memset(cPayname     , 0 , sizeof(cPayname));
    memset(cCashactno   , 0 , sizeof(cCashactno));
    memset(cCashname    , 0 , sizeof(cCashname));
    memset(cOcashname   , 0 , sizeof(cOcashname));
    memset(cOrbrno      , 0 , sizeof(cOrbrno));
    memset(cKinbr       , 0 , sizeof(cKinbr));
    memset(cTxamt       , 0 , sizeof(cTxamt));
    memset(cTel         , 0 , sizeof(cTel));
    memset(cBrno        , 0 , sizeof(cBrno));
    memset(cMiya        , 0 , sizeof(cMiya));
    memset(cPono        , 0 , sizeof(cPono));
    pub_base_sysinit();
    vtcp_log("[%s][%d]同志们sp6344开始了\n",__FILE__,__LINE__);
     /*从前台屏幕取值*/
     /*从前台屏幕取值*/
    get_fd_data("1230",(char *)&tis);
    vtcp_log("[%s][%d] tis.bptype    =[%.1s] \n",__FILE__,__LINE__,tis.bptype     );
    vtcp_log("[%s][%d] tis.avbaltype =[%.1s] \n",__FILE__,__LINE__,tis.avbaltype  );
    vtcp_log("[%s][%d] tis.pono      =[%.16s]\n",__FILE__,__LINE__,tis.pono       );
    vtcp_log("[%s][%d] tis.signtype  =[%.1s] \n",__FILE__,__LINE__,tis.signtype   );
    vtcp_log("[%s][%d] tis.reqno     =[%.10s]\n",__FILE__,__LINE__,tis.reqno      );
    vtcp_log("[%s][%d] tis.payactno  =[%.32s]\n",__FILE__,__LINE__,tis.payactno   );
    vtcp_log("[%s][%d] tis.payname   =[%.60s]\n",__FILE__,__LINE__,tis.payname    );
    vtcp_log("[%s][%d] tis.cashactno =[%.32s]\n",__FILE__,__LINE__,tis.cashactno  );
    vtcp_log("[%s][%d] tis.cashname  =[%.60s]\n",__FILE__,__LINE__,tis.cashname   );
    vtcp_log("[%s][%d] tis.reason    =[%.60s]\n",__FILE__,__LINE__,tis.reason     );
    vtcp_log("[%s][%d] tis.mac       =[%.20s]\n",__FILE__,__LINE__,tis.mac);
    
    memcpy(cTemp,tis.pono,sizeof(tis.pono));
    memcpy(cMiya,tis.mac,sizeof(tis.mac));
    get_zd_data("0030",cKinbr);        /*交易行号*/
    get_zd_long("0440",&lSignday);     /*签发日期*/
    get_zd_data("0270",cOcashname);
    vtcp_log("[%s][%d]lSignday=[%d]\n",__FILE__,__LINE__,lSignday);
    get_zd_data("0070",cTel);          /*签发日期*/
    vtcp_log("[%s][%d]cTel=[%s]\n",__FILE__,__LINE__,cTel);
    get_zd_data("0390",cTxamt);     /*出票金额*/
    vtcp_log("[%s][%d]cTxamt=[%s]\n",__FILE__,__LINE__,cTxamt);
    str2dbl(cTxamt,strlen(cTxamt),0,&dTxamt);
    vtcp_log("[%s][%d]dTxamt=[%f]\n",__FILE__,__LINE__,dTxamt);
    vtcp_log("[%s][%d]cOcashname=[%s]\n",__FILE__,__LINE__,cOcashname);
    
    /***判断是否是清算中心***/
    pub_base_strpack(cKinbr);
    vtcp_log("[%s][%d]cKinbr=[%s]\n",__FILE__,__LINE__,cKinbr);
    if(memcmp(cKinbr,QS_BR_NO,5)==0)
    {
        vtcp_log("%s,%d 清算中心不可以兑付本票",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"清算中心不可以兑付本票",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    /***判断交易行是否存在***/
    iRc=Com_branch_Sel(g_pub_tx.reply,&Com_branch,"br_no='%s' ",cKinbr);
    if(iRc!=0)
    {
        vtcp_log("%s,%d 无此交易机构[%d]",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"[%s][%d]无此交易机构[%d]\n",__FILE__,__LINE__,iRc);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B357");/*B357待定*/
        goto ErrExit;
    }
    
    /**银行本票前补8个零**/
    memcpy(cPono,"00000000000000000000",sizeof(cPono)-1);
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);
    memcpy(cPono+4,cTemp,sizeof(cTemp)-1);          /* 20110422 [8->4] */
    vtcp_log("[%s][%d]cPono==[%s]  ",__FILE__,__LINE__,cPono);
    /**检查本票信息**/
    iRc=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%ld' ",cPono,lSignday);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Dec_Upd错误==[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"此本票号不存在\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    iRc=Cashier_Fet_Upd(&Cashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Fet_Upd错误==[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"此本票号不存在\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    
    lEndday = Cashier.endday;
    vtcp_log("[%s][%d]lEnday=[%ld]\n",__FILE__,__LINE__,lEndday);
    g_reply_int=pub_base_iJumpHoliday(&lEndday);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]得到本票到期日错误[%d]",__FILE__,__LINE__,g_reply_int);
        sprintf(acErrMsg,"得到本票到期日错误");
        Cashier_Debug(&Cashier);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]lEnday=[%ld]\n",__FILE__,__LINE__,lEndday);
    
    if(lEndday < g_pub_tx.tx_date)
    {
        vtcp_log("[%s][%d][%d]该本票已经到期，不允许兑付",__FILE__,__LINE__,Cashier.endday);
        sprintf(acErrMsg,"该本票已经到期，不允许兑付",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    if(pub_base_CompDblVal(Cashier.avbal,dTxamt)!=0)
    {
        vtcp_log("[%s][%d]您输入的交易金额错误==[%f]",__FILE__,__LINE__,Cashier.avbal);
        sprintf(acErrMsg,"您输入的交易金额错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    if(memcmp(Cashier.bptype ,tis.bptype ,sizeof(tis.bptype    ))!=0)
    {
        vtcp_log("[%s][%d]您输入的本票类型错误==[%c]",__FILE__,__LINE__,Cashier.bptype[0]);
        sprintf(acErrMsg,"您输入的本票类型错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    zip_space(cMiya);
    if(memcmp(Cashier.passwd ,cMiya ,sizeof(Cashier.passwd  )-1)!=0)
    {
        vtcp_log("[%s][%d]您输入的密押错误==[%s][%s]",__FILE__,__LINE__,Cashier.passwd,cMiya);
        sprintf(acErrMsg,"您输入的密押错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    /****暂时不用
    if(memcmp(Cashier.ovocnum    ,tis.reqno     ,sizeof(tis.reqno     ))!=0)
    {
        vtcp_log("[%s][%d]您输入的申请书号码错误==[%s]",__FILE__,__LINE__,Cashier.ovocnum);
        sprintf(acErrMsg,"您输入的申请书号码错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B357");
        goto ErrExit;
    }
    ****/
    memcpy(cPayactno,tis.payactno,sizeof(tis.payactno ));
    pub_base_strpack(cPayactno);
    pub_base_strpack(Cashier.payactno);
    if(memcmp(Cashier.payactno ,cPayactno ,strlen(Cashier.payactno))!=0 || (strlen(Cashier.payactno)!=strlen(cPayactno)))
    {
        vtcp_log("[%s][%d]付款帐户错误==[%s][%s]",__FILE__,__LINE__,Cashier.payactno,cPayactno);
        sprintf(acErrMsg,"您输入的付款帐户错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B357");/**B357待定**/
        goto ErrExit;
    }
    memcpy(cPayname,tis.payname,sizeof(tis.payname  ));
    pub_base_strpack(cPayname);
    pub_base_strpack(Cashier.payname);
    if(memcmp(Cashier.payname ,cPayname ,strlen(cPayname))!=0 || strlen(cPayname)!=strlen(Cashier.payname))
    {
        vtcp_log("[%s][%d]您输入的付款户名错误==[%s]  ",__FILE__,__LINE__,Cashier.payname);
        sprintf(acErrMsg,"您输入的付款户名错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    if(Cashier.stat[0]!='1')
    {
        vtcp_log("[%s][%d]不是签发状态,不允许兑付=[%c]  ",__FILE__,__LINE__,Cashier.stat[0]);
        sprintf(acErrMsg,"[%s][%d]不是签发状态,不允许兑付",__FILE__,__LINE__);
        WRITEMSG
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    memcpy(cBrno,Cashier.brno,sizeof(cBrno)-1);/**签发机构**/
    /**判断兑付行是否是我行**/
    if(GetOr_br_noByBr_no(cKinbr,cOrbrno)!=0)
    {
        vtcp_log("[%s][%d] 根据机构号查找参与者行号错误[%s]\n",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"根据机构号查找参与者行号错误",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B357");/**B357待定**/
        goto ErrExit;
    }
    vtcp_log("[%s][%d]最后兑付行为:[%s]\n",__FILE__,__LINE__,cOrbrno);
    pub_base_strpack(cOrbrno);
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab,"or_br_no='%s' ",cOrbrno);
    if(iRc!=0)
    {
        vtcp_log("%s,%d 查询发起行参与者行号错误[%s]",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"[%s][%d]查询发起行参与者行号错误",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    if(tis.bptype[0] =='1')/**如果是现金本票**/
    {
          if(memcmp(Hv_uniontab.qs_no,HV_QS_BR_NO,sizeof(Hv_uniontab.qs_no)-1)!=0)
          {
              vtcp_log("%s,%d 现金本票必须是本行兑付[%s]",__FILE__,__LINE__,cOrbrno);
              sprintf(acErrMsg,"[%s][%d]现金本票必须是本行兑付",__FILE__,__LINE__);
              set_zd_data(DC_GET_MSG,acErrMsg);
              strcpy(g_pub_tx.reply,"8888");/**B357待定**/
              goto ErrExit;
          }
          
          memcpy(cCashactno,tis.cashactno,sizeof(tis.cashactno ));
          pub_base_strpack(cCashactno);
          pub_base_strpack(Cashier.cashactno);
          if(memcmp(Cashier.cashactno ,cCashactno ,strlen(Cashier.cashactno))!=0 || (strlen(Cashier.cashactno)!=strlen(cCashactno)))
          {
              vtcp_log("[%s][%d]收款帐户错误==[%s][%s]",__FILE__,__LINE__,Cashier.cashactno,cCashactno);
              sprintf(acErrMsg,"您输入的收款帐户错误\n",__FILE__,__LINE__);
              set_zd_data(DC_GET_MSG,acErrMsg);
              strcpy(g_pub_tx.reply,"8888");/**B357待定**/
              goto ErrExit;
          }
          memcpy(cCashname,tis.cashname,sizeof(tis.cashname  ));
          pub_base_strpack(cCashname);
          pub_base_strpack(Cashier.cashname);
          if(memcmp(Cashier.cashname ,cCashname ,strlen(cCashname))!=0 || strlen(cCashname)!=strlen(Cashier.cashname))
          {
              vtcp_log("[%s][%d]您输入的收款户名错误==[%s]  ",__FILE__,__LINE__,Cashier.cashname);
              sprintf(acErrMsg,"您输入的收款户名错误\n",__FILE__,__LINE__);
              set_zd_data(DC_GET_MSG,acErrMsg);
              strcpy(g_pub_tx.reply,"8888");/**B357待定**/
              goto ErrExit;
          }
    }
    else/**现在是转账本票,如果收款人不对那么我判断是否原收款人名称正确呢？**/
    {
    	    vtcp_log("[%s][%d]cCashname=[%s]\n",__FILE__,__LINE__,cCashname);
          memcpy(cCashname,tis.cashname,sizeof(tis.cashname  ));
          pub_base_strpack(cCashname);
          pub_base_strpack(Cashier.cashname);
          if(memcmp(Cashier.cashname ,cCashname ,strlen(cCashname))!=0 || strlen(cCashname)!=strlen(Cashier.cashname))
          {
              if(memcmp(Cashier.cashname , cOcashname,strlen(Cashier.cashname))!=0 || (strlen(Cashier.cashname)!=strlen(cOcashname))) /***NEWBP3****/
          	  {
          	  	  vtcp_log("[%s][%d]收款帐户错误==[%d][%d]",__FILE__,__LINE__,strlen(Cashier.cashname),strlen(cOcashname));
                  vtcp_log("[%s][%d]收款帐户错误==[%s][%s]",__FILE__,__LINE__,Cashier.cashname,cOcashname);
                  sprintf(acErrMsg,"您输入的原收款帐户错误\n",__FILE__,__LINE__);
                  set_zd_data(DC_GET_MSG,acErrMsg);
                  strcpy(g_pub_tx.reply,"8888");/**B357待定**/
                  goto ErrExit;
              }
              vtcp_log("[%s][%d]背书成功!!!\n",__FILE__,__LINE__);
          }
    }
    vtcp_log("[%s][%d]准备开始更新表记录\n",__FILE__,__LINE__);
    Cashier.lactcls[0] = Cashier.stat[0];
    Cashier.stat[0] = '2';             /**全额兑付**/
    Cashier.cashday = g_pub_tx.tx_date;/**兑付日期**/
    memcpy(Cashier.cashtlrno,cTel,sizeof(Cashier.cashtlrno)-1);
    memcpy(Cashier.jqtlrno,cTel,sizeof(Cashier.jqtlrno)-1);
    /**最后持票人**/
    memcpy(Cashier.holdactno  ,tis.cashactno ,sizeof(tis.cashactno ));
    memcpy(Cashier.holdname  ,tis.cashname  ,sizeof(tis.cashname  ));
    memcpy(Cashier.holdbrno   ,cOrbrno       ,sizeof(Cashier.holdbrno)-1);
    memcpy(Cashier.holdbrname   ,Hv_uniontab.br_name_f,sizeof(Cashier.holdname)-1);
    if(memcmp(Cashier.brno,cKinbr,sizeof(cKinbr)-1)==0)
    {
        Cashier.cashstat[0]='0'; /**0.系统内本行兑付本行签发**/
    }
    else
    {
        Cashier.cashstat[0]='1'; /**1.系统内他行兑付本行签发**/
    }
    memcpy(Cashier.filler ,tis.reason ,sizeof(Cashier.filler)-1);
    /***开始生成本票记录***/
    vtcp_log("[%s][%d]准备更新表了\n",__FILE__,__LINE__);
    iRc=Cashier_Upd_Upd(Cashier,g_pub_tx.reply );
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]修改本票记录错误[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"修改本票记录错误",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    Cashier_Clo_Upd();
    vtcp_log("[%s][%d]更新本票记录完毕\n",__FILE__,__LINE__);
    /**登记交易流水**/
    memcpy(g_pub_tx.beg_note_no,tis.pono,16); /***NEWBP3**/   /* 20110422 [12->16] */
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]登记交易流水出错[%d]  ",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"登记交易流水出错",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");/**B357待定**/
        goto ErrExit;
    }
    /**检查签发行的兑付帐号**/
    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=88",cBrno);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]承兑付款临时帐户错误!cnt=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W025");
        WRITEMSG
        goto ErrExit;
    }
    /** 检查本票签发户是否存在 **/
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]检查本票签发户失败!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    ret = In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]本票签发户错误!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    if( in_mst.sts[0]!='1' && in_mst.sts[0]!='2' )
    {
        sprintf(acErrMsg,"账号状态非正常状态![%ld,%s]",in_mst.ac_id,in_mst.sts);
        strcpy(g_pub_tx.reply,"P109");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data( "0300", com_parm.val);
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
