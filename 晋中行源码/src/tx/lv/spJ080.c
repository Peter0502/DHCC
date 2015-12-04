/************************************************************
* 文 件 名:   spJ080.c
* 功能描述：  小额业务错误来帐处理
*                记帐变为STAT_LZRECV2 'I' 状态,修改不改状态，因为可能不修改直接入帐
* 作    者:   xyy
* 完成日期：  2006-10-18 14:45
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"

static PAY_IN_AREA payin,payin_modif;
extern void lv_fullspace_len(char * con,int len);
extern char *pcLvGetDay();
static char *Getywname(char *);
static void fullspace_len(char * con,int len);
static char cQsbrno[BRNO_LEN+1];
static char cInBrno[BRNO_LEN+1];
int spJ080()
{

    int ret;
    char cTxamt[17];/* add by LiuHuafeng 2006-10-20 19:28 */
    char cCash_ac_no[33];
    memset(cCash_ac_no , 0 , sizeof(cCash_ac_no));
    memset(cInBrno     , 0 , sizeof(cInBrno));
    memset(cQsbrno     , 0 , sizeof(cQsbrno));
    memset(&payin      , 0 , sizeof(payin));
    memset(&payin_modif, 0 , sizeof(payin_modif));
    vtcp_log("[%s][%d]spJ080小额业务错帐处理交易BEGIN",__FILE__,__LINE__);
    get_fd_data( "0030", cInBrno);           /** 取得交易机构 **/

    /*** 初始化全局变量 ***/
    pub_base_sysinit();
    if(iLvchkstat())
    {
        strncpy(g_pub_tx.reply,"P046",4);
        WRITEMSG
        goto ErrExit;
    }

    /****填充payin*********/
    g_reply_int = iLvTisToPayin(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"调用iLvTisToPayin出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] F_opcd=[%s]",__FL__,payin.F_opcd);


    /* 检查输入的来帐信息是否存在 */
    g_reply_int = iLvFetchRecToPayin(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"调用iLvFetchRecToPayin出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    /* add by LiuHuafeng 2006-10-20 19:30 准备流水中的交易金额*/
    memcpy(cTxamt,payin.F_txamt,sizeof(payin.F_txamt));
    zip_space(cTxamt);
    str2dbl(cTxamt,strlen(cTxamt),0,&g_pub_tx.tx_amt1);
    g_pub_tx.tx_amt1/=100;
    /** end by LiuHuafeng 2006-10-20 19:30 */

    /**保存payin信息**/
    memcpy((char*)&payin_modif, (char*)&payin,sizeof(payin_modif));

    /**根据payin结构体信息反填**/
    iLvResetPayin(&payin);
    g_reply_int = iLvPayinCheck0(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"基本检查出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }

    /*** 取得清算中心机构号 ***/
    memcpy(cQsbrno,QS_BR_NO,BRNO_LEN);

    if(memcmp(cInBrno,cQsbrno,BRNO_LEN)!=0)/**如果不是清算中心**/
    {
        if(memcmp(cInBrno,payin.T_brno,BRNO_LEN)!=0)
        {
            sprintf( acErrMsg,"不能代他机构进行处理,[%s][%d]cInBrno,T_brno", __FILE__,__LINE__,cInBrno,payin.T_brno);
            strcpy( g_pub_tx.reply, "S043" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
    }

    /* 检查该笔是否需要进行错帐处理 */
    if(payin.T_stat[0]!=payin.T_begstat[0])
    {
        vtcp_log("[%s][%d]该笔业务已经进行过其他处理stat=[%c],begstat=[%c]!\n",__FILE__,__LINE__,payin.T_stat[0],payin.T_begstat[0]);
        sprintf( acErrMsg,"该笔业务已经进行过其他处理,不可以错帐处理!,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "P077" );
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    if(payin.T_stat[0]!=STAT_LZGZ_QS)
    {
    /*** 清算中心也是一个机构，也存在支行挂账的情况
        if(memcmp(cInBrno,QS_BR_NO,BRNO_LEN)==0)
        {
            sprintf( acErrMsg,"支行挂帐,清算中心不能处理[%s][%d]cInBrno,T_brno", __FILE__,__LINE__,cInBrno);
            strcpy( g_pub_tx.reply, "S043" );
            WRITEMSG
            goto ErrExit;
        }
    ********/
    }
    /********************add by xyy 2009-10-12 9:57:43 *************************/
    if(payin.T_stat[0]!=STAT_LZNMAC_ERR && payin.T_stat[0]!=STAT_LZLMAC_ERR && payin.T_stat[0]!=STAT_LZAMAC_ERR\
       && payin.T_stat[0]!=STAT_LZGZ_QS && payin.T_stat[0]!=STAT_LZGZ_BR )
    {
        sprintf( acErrMsg,"非挂账状态不能处理!");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy( g_pub_tx.reply, "S043" );
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] T_rcpstat=[%s] T_pkgno=[%.3s]",__FILE__,__LINE__,payin.T_rcpstat,payin.T_pkgno);
    if(memcmp(payin.T_pkgno,"008", 3)==0 || memcmp(payin.T_pkgno,"010",3)==0 || \
        memcmp(payin.T_pkgno,"011",3)==0)
    {
        if(memcmp(payin.T_rcpstat,"00",2)!=0)
        {
            sprintf( acErrMsg,"对方没有回执成功还去做错账处理???");
            set_zd_data(DC_GET_MSG,acErrMsg);
            strcpy( g_pub_tx.reply, "S043" );
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("[%s][%d] T_rcpstat=[%s]",__FILE__,__LINE__,payin.T_rcpstat);
    }
    if(memcmp(payin.T_pkgno,"002", 3)==0 || memcmp(payin.T_pkgno,"004",3)==0 || \
        memcmp(payin.T_pkgno,"006",3)==0 || memcmp(payin.T_pkgno,"009",3)==0 )
    {
        sprintf( acErrMsg,"不能处理此类报文!");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy( g_pub_tx.reply, "S043" );
        WRITEMSG
        goto ErrExit;
    }
    /******************end by xyy 2009-10-12 9:58:00**********************/
    vtcp_log("[%s][%d] opcd==[%.2s]",__FILE__,__LINE__,payin.F_opcd);
    vtcp_log("[%s][%d] pkgno===[%s]",__FILE__,__LINE__,payin.F_pkgno);
    if(memcmp(payin.F_opcd,OPCD_INACT_MODIFY,sizeof(payin.F_opcd))==0)
    {
        memcpy(payin.F_cashactno,payin_modif.F_cashactno,sizeof(payin.F_cashactno));
        memcpy(payin.F_cashname ,payin_modif.F_cashname,sizeof(payin.F_cashname));
        memcpy(payin.T_cashactno,payin_modif.F_cashactno,sizeof(payin.F_cashactno));
        memcpy(payin.T_cashname ,payin_modif.F_cashname,sizeof(payin.F_cashname));
        memcpy(payin.T_filler   ,cInBrno,BRNO_LEN);/**入帐机构**/
        memcpy(payin.T_filler+BRNO_LEN,pcLvGetDay(),8);/**入帐时间**/
        get_zd_data("0040",payin.T_inwssrno);/* 流水号*/
        get_zd_data("0070",payin.T_inputno);/* 操作员*/
        vtcp_log("[%s][%d] T_inwssrno==[%.6s]",__FILE__,__LINE__,payin.T_inwssrno);
        vtcp_log("[%s][%d] T_inputno===[%.6s]",__FILE__,__LINE__,payin.T_inputno);
        g_reply_int = iLvUpdRecFromPayin(&payin,0);
        if(g_reply_int)
        {
            vtcp_log("%s,%d,更新登记簿失败,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"更新登记簿失败,[%s][%d]", __FILE__,__LINE__);
            strcpy( g_pub_tx.reply, "HV01" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        /* 记流水日志 */
        g_reply_int = pub_ins_trace_log();
        if ( g_reply_int )
        {
            sprintf( acErrMsg, "记流水日志错" );
            strcpy( g_pub_tx.reply, "HV01" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        /* 返回给前台 */
        g_reply_int = ilv10_PreTos();
        if(g_reply_int)
        {
            sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
            strcpy( g_pub_tx.reply, "HV01" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        goto OkExit;
    }
    else if(memcmp(payin.F_opcd,OPCD_INACT,sizeof(payin.F_opcd))==0)
    {
        memcpy(cCash_ac_no,payin.T_cashactno,sizeof(payin.T_cashactno));
        vtcp_log("[%s][%d],cCash_ac_no=[%s]",__FILE__,__LINE__,cCash_ac_no);
        zip_space(cCash_ac_no);
        vtcp_log("[%s][%d],cCash_ac_no=[%s]",__FILE__,__LINE__,cCash_ac_no);
        if(memcmp(cCash_ac_no,"10101",strlen(cCash_ac_no))==0)
        {
              vtcp_log("[%s][%d],收款帐号为现金科目,须指定一帐号才能入帐",__FILE__,__LINE__);
              sprintf( acErrMsg,"收款帐号为现金科目,须指定一帐号才能入帐", __FILE__,__LINE__);
              strcpy( g_pub_tx.reply, "HV01" );
              set_zd_data(DC_GET_MSG,acErrMsg);
              WRITEMSG
              goto ErrExit;
        }
        payin.T_stat[0]=STAT_LZRECV2;
        get_zd_data("0040",payin.T_dealwssrno);/* 流水号*/
        get_zd_data("0070",payin.T_dealno);/* 操作员*/
        vtcp_log("[%s][%d] T_inwssrno==[%.6s]",__FILE__,__LINE__,payin.T_inwssrno);
        vtcp_log("[%s][%d] T_inputno===[%.6s]",__FILE__,__LINE__,payin.T_inputno);
        g_reply_int = iLvUpdRecFromPayin(&payin,0);
        if(g_reply_int)
        {
            vtcp_log("[%s][%d],调用iLvUpdRecFromPayin失败,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"调用iLvUpdRecFromPayin失败,[%s][%d]", __FILE__,__LINE__);
            strcpy( g_pub_tx.reply, "HV01" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        /* 记流水日志 */
        g_reply_int = pub_ins_trace_log();
        if ( g_reply_int )
        {
            sprintf( acErrMsg, "记流水日志错" );
            strncpy(g_pub_tx.reply,"S023",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        /* 返回给前台 */
        g_reply_int = ilv10_PreTos();
        if(g_reply_int)
        {
            sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
            strncpy(g_pub_tx.reply,"P079",4);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        goto OkExit1;
    }
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit1:
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"准备走帐 return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

char *Getywname(char * pkgno)
{
     char ywname[21];
     char cPkgno[4];
     int  iPkgno=0;

    memset(ywname,'\0',sizeof(ywname));
    memset(cPkgno, 0, sizeof(cPkgno));

    memcpy(cPkgno, payin.F_pkgno, sizeof(cPkgno)-1);
    iPkgno = apatoi(cPkgno,3);
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__,iPkgno);
    switch(iPkgno)
    {
        case 1:
        memcpy(ywname,"小额普贷来帐",sizeof(ywname)-1);
        break;
        case 5:
        memcpy(ywname,"小额定贷来帐",sizeof(ywname)-1);
        break;
        case 7:
        memcpy(ywname,"小额贷退来帐",sizeof(ywname)-1);
        break;
        case  8:
        memcpy(ywname,"小额普回来帐",sizeof(ywname)-1);
        break;
        case 11:
        memcpy(ywname,"小额定回来帐",sizeof(ywname)-1);
        break;
        default:
                vtcp_log("[%s][%d]错误的业务类型！",__FILE__,__LINE__);
            sprintf( acErrMsg,"错误的业务类型,[%s][%d]", __FILE__,__LINE__);
            strcpy( g_pub_tx.reply, "HV01" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            break;
    }
    vtcp_log("[%s][%d][%s]",__FILE__,__LINE__,ywname);
    return ywname;
}
static void lv_fullspace_len(char * con,int len)
{
    char cTmpbuf[240];
    int i=0,iLen=0;
    memset(cTmpbuf, 0 , sizeof(cTmpbuf));
    memcpy(cTmpbuf,con,sizeof(cTmpbuf)-1);
    if(len<sizeof(cTmpbuf)-1) iLen=len;
    else iLen=sizeof(cTmpbuf)-1;
    for(i=0;i<iLen;i++)
    {
        if(cTmpbuf[i]=='\0') cTmpbuf[i]=' ';
    }
    memcpy(con,cTmpbuf, len);
}
ilv10_PreTos()
{
    int iRc;
    /****构造输出内容payout2****/
    iLvSetPayout1(&payin);
    return(0);
}
