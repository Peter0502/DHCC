/**************************************************************************/
/*文件名称: sp6343.c                                                      */
/*功能描述: 签发本票信息的签发                                            */
/*作    者: xyy    2008-02-25                                             */
/**************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "hv_uniontab_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_parm_c.h"
extern int nCountBpMac(char *pcPayNo,char *pcPayActNo,double dTxamt,char *pcVocNo,char *pcSignDay,char *pcMac);
int sp6343()
{
    int     iRc=0;
    int     ret=0;
    char    cTel     [5 ];
    char    cMiya    [21];
    char    cPono    [17];  /* 20110422 [13->17] */
    char    lPono    [17];  /***NEWBP3给流水准备的****/ /* 20110422 [13->17] */
    char    cKinbr   [6 ];
    char    cHyflag  [2 ];
    char    cTemp    [21];
    char    cOrbrno  [13];
    char    cPayactno[33];
    char    cTxamt   [17];
    char    cSignday [9 ];
    char    cVocNum  [17] ;
    char    cVtype   [4];
    char    cC_VocNum[17] ;
    
    long    lQfDay=0;
    double  dTx_amt=0.0;
    struct  cashier_c      cCashier;     /***本票基本信息表***/
    struct  hv_uniontab_c  cHv_uniontab; /***行名行号信息表***/
    struct  com_branch_c   cCom_branch;  /***机构表***/
    struct  com_parm_c     com_parm;
    struct  mdm_ac_rel_c   mdm_ac_rel;
    struct  in_mst_c       in_mst;
    /****初始化变量****/
    memset(cTel        , 0 , sizeof(cTel));
    memset(cMiya       , 0 , sizeof(cMiya));
    memset(cPono       , 0 , sizeof(cPono));
    memset(lPono       , 0 , sizeof(lPono));   /***NEWBP3给流水准备的****/
    memset(cKinbr      , 0 , sizeof(cKinbr));
    memset(cTemp       , 0 , sizeof(cTemp));
    memset(cVocNum     , 0 , sizeof(cVocNum));
    memset(cHyflag     , 0 , sizeof(cHyflag));
    memset(cTxamt      , 0 , sizeof(cTxamt));
    
    memset(cVtype      , 0 , sizeof(cVtype));
    memset(cC_VocNum   , 0 , sizeof(cC_VocNum));
    
    memset(cOrbrno     , 0 , sizeof(cOrbrno));
    memset(cSignday    , 0 , sizeof(cSignday));
    memset(cPayactno   , 0 , sizeof(cPayactno));
    memset(&cCom_branch ,0 , sizeof(cCom_branch));
    memset(&cCashier    ,0 , sizeof(cCashier));
    memset(&com_parm    ,0 , sizeof(com_parm));
    memset(&in_mst      ,0 , sizeof(in_mst));
    memset(&mdm_ac_rel  ,0 , sizeof(mdm_ac_rel));
    memset(&cHv_uniontab,0 , sizeof(cHv_uniontab));
    pub_base_sysinit();
    vtcp_log("[%s][%d]同志们sp6343开始了\n",__FILE__,__LINE__);
    /*从前台屏幕取值*/
    get_zd_data("0320",cPono  );    /*本票号码*/
    get_zd_data("0320",lPono  );    /*本票号码NEWBP3给流水准备的*/
    get_zd_long("0450",&lQfDay );    /*签发日期*/
    get_zd_data("0720",cHyflag);    /*本票加压方式0机器1人工*/
    get_zd_data("0630",cMiya  );    /*本票密押*/
    get_zd_data("0030",cKinbr );    /*交易行号*/
    get_zd_data("0070",cTel   );    /*签发操作员*/
    get_zd_data("0390",cTxamt);     /*出票金额*/
    get_zd_data("0600",cVocNum);    /*柜员凭证号码*/
    get_zd_data("0610",cC_VocNum);  /*客户凭证号码*/
    get_zd_data("0900",cVtype);     /*客户凭证种类*/
    
    vtcp_log("[%s][%d]cVocNum===[%s]\n",__FILE__,__LINE__,cVocNum);
    vtcp_log("[%s][%d] cPono   =[%s]\n",__FILE__,__LINE__,cPono   );
    vtcp_log("[%s][%d] lQfDay  =[%ld]\n",__FILE__,__LINE__,lQfDay );
    vtcp_log("[%s][%d] cHyflag =[%s]\n",__FILE__,__LINE__,cHyflag );
    vtcp_log("[%s][%d] cMiya   =[%s]\n",__FILE__,__LINE__,cMiya   );
    vtcp_log("[%s][%d] cKinbr  =[%s]\n",__FILE__,__LINE__,cKinbr  );
    vtcp_log("[%s][%d] cTel    =[%s]\n",__FILE__,__LINE__,cTel    );
    vtcp_log("[%s][%d] cTxamt  =[%s]\n",__FILE__,__LINE__,cTxamt  );
    
    str2dbl(cTxamt,strlen(cTxamt),0,&dTx_amt);
    vtcp_log("[%s][%d] dTx_amt  =[%f]\n",__FILE__,__LINE__,dTx_amt);
    
    /**判断柜员凭证是否和本票号码一致**/
    /* if(memcmp(cVocNum+8,cPono+4, 8)!=0) 20110422 */
    if(memcmp(cVocNum,cPono, 16)!=0)       /* 20110422 */
    {
        vtcp_log("[%s][%d]凭证号码[%s]和本票号码[%s]不符\n",__FILE__,__LINE__,cVocNum,cPono);
        sprintf(acErrMsg,"凭证号码和本票号码不符!");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strncpy(g_pub_tx.reply,"8888",4);
        goto ErrExit;
    }
    /***判断是否是清算中心***/
    if(memcmp(cKinbr,QS_BR_NO,5)==0)
    {
        vtcp_log("[%s][%d]清算中心不可以修改本票[%s]\n",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"清算中心不可以修改本票",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***判断交易行是否存在***/
    iRc=Com_branch_Sel(g_pub_tx.reply,&cCom_branch,"br_no='%s' ",cKinbr);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]无此交易机构[%s]\n",__FILE__,__LINE__,cKinbr);
        sprintf(acErrMsg,"无此交易机构",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***根据交易行号,得到参与者行号***/
    if(GetOr_br_noByBr_no(cKinbr,cOrbrno)!=0)
    {
        vtcp_log("[%s][%d]根据机构号查找参与者行号错误[%s]\n",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"根据机构号查找参与者行号错误",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*查找hv_uniotab,确定参与者行号存在*/
    iRc=Hv_uniontab_Sel(g_pub_tx.reply,&cHv_uniontab,"or_br_no='%s' ",cOrbrno);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d] 查询发起行参与者行号错误[%s]\n",__FILE__,__LINE__,cOrbrno);
        sprintf(acErrMsg,"查询发起行参与者行号错误",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    memcpy(cTemp,"00000000000000000000",sizeof(cTemp)-1);
    vtcp_log("[%s][%d]cTemp==[%s]\n",__FILE__,__LINE__,cTemp);
    memcpy(cTemp+4,cPono,sizeof(cPono)-1);      /* 20110422 [8 -> 4] */
    vtcp_log("[%s][%d]cTemp==[%s]\n",__FILE__,__LINE__,cTemp);
    iRc=Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%ld' ",cTemp,lQfDay);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Dec_Upd错误==[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"此本票号不存在\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    iRc=Cashier_Fet_Upd(&cCashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]Cashier_Fet_Upd错误==[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"此本票号不存在\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*本票余额不可以为零*/
    if(pub_base_CompDblVal(cCashier.avbal,0.00)==0)
    {
        vtcp_log("[%s][%d]本票金额不能为零==[%f]\n",__FILE__,__LINE__,cCashier.avbal);
        sprintf(acErrMsg,"本票金额不能为零!\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***开始修改本票记录***/
    if(memcmp(cCashier.optlrno,cTel,sizeof(cCashier.optlrno)-1)==0)
    {
        vtcp_log("[%s][%d]签发和录入不能是同一操作员=[%s]\n",__FILE__,__LINE__,cTel);
        sprintf(acErrMsg,"录入和签发不能同一个操作员!");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strncpy(g_pub_tx.reply,"P080",4);
        goto ErrExit;
    }
    if(cCashier.stat[0]!='0')/**本票状态不是登记状态不能签发**/    
    {
        vtcp_log("[%s][%d][%c]本票不是登记状态[0]\n",__FILE__,__LINE__,cCashier.stat[0]);
        sprintf(acErrMsg,"本票不是登记状态",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(cCashier.endday<g_pub_tx.tx_date)
    {
        vtcp_log("[%s][%d][%c]签发日期大于本票到期日[0]\n",__FILE__,__LINE__,cCashier.endday);
        sprintf(acErrMsg,"签发日期大于本票到期日\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    if(pub_base_CompDblVal(cCashier.avbal,dTx_amt)!=0)
    {
    	  vtcp_log("[%s][%d][%f]交易金额不符[%f]\n",__FILE__,__LINE__,cCashier.avbal,dTx_amt);
        sprintf(acErrMsg,"交易金额不符\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"B103");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]准备开始更新表记录\n",__FILE__,__LINE__);
    cCashier.checkflag[0]='0';               /**对帐标志**/
    cCashier.lactcls[0]=cCashier.stat[0];    /**上次账户状态**/
    cCashier.stat[0]='1';                    /**签发状态**/
    cCashier.regday=g_pub_tx.tx_date;        /**签发日期**/
    memcpy(cCashier.chktlrno,cTel,sizeof(cCashier.chktlrno)-1);
    
    
    /***增加检查业务委托书凭证***/
    pub_base_strpack(cC_VocNum);
    vtcp_log("[%s][%d]开始凭证处理 type[%s][%s]\n",__FILE__,__LINE__,cVtype,cC_VocNum);

    /*检查凭证种类是否可用,本票申请书号码采用了业务委托书的凭证*/
    iRc=pub_com_ChkNoteUse(cVtype);
    if( iRc )
    {
        sprintf(acErrMsg,"凭证处于停用状态!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"N085");
        goto ErrExit;
    }
    vtcp_log("[%s][%d] 凭证基本检查开始\n",__FILE__,__LINE__);
    /**如果帐户长度大于7位,且凭证种类为业务委托书**/
    /**得到付款帐号**/
    memcpy(cPayactno,cCashier.payactno,sizeof(cCashier.payactno)-1);
    vtcp_log("[%s][%d]cPayactno=[%s]\n",__FILE__,__LINE__,cPayactno);
    pub_base_strpack(cPayactno);
    if(strlen(cPayactno)>7 && memcmp(cVtype,"005",sizeof(cVtype)-1)==0)
    {
        g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",cPayactno);
        if(g_reply_int)
        {
            sprintf(acErrMsg,"帐号查询出错!");
            strcpy(g_pub_tx.reply,"B117");
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("[%s][%d]Ms_mdm_ac_rel.ac_id=[%ld],cVocnum=[%s]\n",__FILE__,__LINE__,mdm_ac_rel.ac_id,cC_VocNum);
        vtcp_log("开始凭证处理 type[%s][%s][%s][%d]\n",cVtype,cC_VocNum,__FILE__,__LINE__);
        g_reply_int=pub_com_NoteUse(mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn,cVtype,cC_VocNum,cC_VocNum,g_pub_tx.tel);
        if(g_reply_int)
        {
            sprintf(acErrMsg,"处理凭证出错!");
            strcpy(g_pub_tx.reply,"N080");
            WRITEMSG
            goto ErrExit;
        }
    }
    /** 凭证处理结束 **/
    vtcp_log("[%s][%d]客户凭证处理结束\n",__FILE__,__LINE__);
    memcpy(cCashier.ovoctype,cVtype,sizeof(cCashier.ovoctype)-1); /**客户凭证种类**/
    memcpy(cCashier.ovocnum,cC_VocNum,sizeof(cCashier.ovocnum)-1);/**客户凭证号码**/
    
    /**************检测柜员的银行本票凭证********************/
    ret=pub_com_ChkNoteUse("186");/*检查凭证种类是否可用,本票签发采用了186凭证*/
    if( ret )
    {
        sprintf(acErrMsg,"凭证处于停用状态!");
        strcpy(g_pub_tx.reply,"N085");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] cVocunm==[%s],cTel==[%s]\n",__FILE__,__LINE__,cVocNum,cTel);
    vtcp_log("[%s][%d] cVocunm==[%s],cTel==[%s]\n",__FILE__,__LINE__,cVocNum,cTel);
    ret=pub_com_NoteUse(0,0,"186",cVocNum,cVocNum,cTel);/*凭证销号处理(交易过程中使用凭证)*/
    if( ret )
    {
        sprintf(acErrMsg,"柜员使用凭证错误!");
        strcpy(g_pub_tx.reply,"N007");
        WRITEMSG
        goto ErrExit;
    }
    memcpy(cCashier.voctype,"186",sizeof(cCashier.voctype)-1);/**凭证种类**/
    memcpy(cCashier.vocnum,cVocNum,sizeof(cCashier.vocnum)-1);/**凭证号码**/
    apitoa(cCashier.signday,8,cSignday);
    /*机器密押*/
    if(cHyflag[0]=='1')/**手工加押**/
    {
        cCashier.addpassflg[0] = '1';
        memcpy(cCashier.passwd,cMiya,sizeof(cCashier.passwd)-1);
    }
    else if(cHyflag[0]=='0')/**机器加押**/
    {
        cCashier.addpassflg[0] = '0';
        vtcp_log("[%s][%d]自动核押\n",__FILE__,__LINE__);
        nCountBpMac(cCashier.orbrno,cCashier.payactno,cCashier.avbal*100,cCashier.pono,cSignday,cCashier.passwd);
        vtcp_log("[%s][%d]cCashier.passwd=[%s]\n",__FILE__,__LINE__,cCashier.passwd);
        
    }
    iRc=Cashier_Upd_Upd(cCashier,g_pub_tx.reply);
    if(iRc!=0)
    {
        vtcp_log("[%s][%d]修改本票记录错误[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"修改本票记录错误\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    Cashier_Clo_Upd();

    /**登记交易流水**/
    vtcp_log("[%s][%d]本票号码cPono=[%s]\n",__FILE__,__LINE__,cPono);
    
    memcpy(g_pub_tx.beg_note_no,lPono,16); /***NEWBP3给流水准备的****/ /* 20110422 [12->16] */
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        vtcp_log("[%s][%d]登记交易流水出错[%d]\n",__FILE__,__LINE__,iRc);
        sprintf(acErrMsg,"登记交易流水出错\n",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /***给返回域赋值(行名)***/
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=88",g_pub_tx.tx_br_no);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]承兑付款临时帐户错误!cnt=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W025");
        WRITEMSG
        goto ErrExit;
    }
    /**检查本票签发户是否存在**/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]检查本票签发户失败!sqlcode=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d' and ac_seqn=0 ",mdm_ac_rel.ac_id);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]本票签发户错误!sqlcode=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    if(in_mst.sts[0]!='1'&&in_mst.sts[0]!='2')
    {
        sprintf(acErrMsg,"账号状态非正常状态![%ld,%s]\n",in_mst.ac_id,in_mst.sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
    }
    set_zd_data("1211",com_parm.val);
    set_zd_data("1214","01");
    set_zd_double("1218",atoi(cTxamt));
    set_zd_data("121A","银行本票签发");
    set_zd_data("0620",cCashier.passwd);
    vtcp_log("[%s][%d]返回密押[%s]!\n",__FILE__,__LINE__,cCashier.passwd);
    set_zd_data("0520",cOrbrno);
    vtcp_log("[%s][%d]程序顺利结束啦!\n",__FILE__,__LINE__);
OkExit:
    strcpy( g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
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
