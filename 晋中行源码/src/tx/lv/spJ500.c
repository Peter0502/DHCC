/*****************************************************
* 文 件 名:  spJ500.c
* 功能描述： 银行本票兑付申请录入
* 作    者:  xyy
* 完成日期： 2008-1-9 16:47 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "lv_wbctl_c.h"
static    PAY_IN_AREA payin;

int spJ500()
{
    int iRc = 0;
    struct lv_wbctl_c lv_wbctl;
    struct lv_pkgreg_c lv_pkgreg;
    memset(&payin,0,sizeof(payin));
    memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
    memset(&lv_wbctl,0,sizeof(lv_wbctl));
    memset(&lvca, 0 , sizeof(lvca));
    vtcp_log("[%s][%d] spJ500开始喽\n",__FILE__,__LINE__);

    pub_base_sysinit();
    vtcp_log("[%s][%d] 检查系统状态\n",__FILE__,__LINE__);
    if(iLvchkstat())
    {
        strncpy(g_pub_tx.reply,"P046",4);
        WRITEMSG
        goto ErrExit;
    }
    /****填充payin*********/
    vtcp_log("[%s][%d] 填充payin结构体\n",__FILE__,__LINE__);
    g_reply_int = iLvTisToPayin(&payin);
    if( g_reply_int )
    {
        sprintf( acErrMsg,"[%s][%d]函数iHvTistoNotpay出错", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "P015" );
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] payin.F_lw_ind = [%c]",__FL__,payin.F_lw_ind[0]);
    /****基本检查****/
    g_reply_int = iLvPayinCheck0(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"基本检查出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /****要素检查****/
    vtcp_log("[%s][%d] 要素检查iLvPayinCheck4\n",__FILE__,__LINE__);
    g_reply_int = iLvPayinCheck4(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"要素检查出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /****行号检查****/
    vtcp_log("[%s][%d] 行号检查iLvPayinCheck1\n",__FILE__,__LINE__);
    g_reply_int = iLvPayinCheck1(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"行号检查出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /* 手续费检查 */
    vtcp_log("[%s][%d] 手续费检查\n",__FILE__,__LINE__);
    iLvCountFeeamt(&payin);
    vtcp_log("[%s][%d] 收款帐号检查iLvPayinCheck3\n",__FILE__,__LINE__);
    /****收款帐号检查****/
    g_reply_int=iLvPayinCheck3(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"收款帐号检查出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] payactno[%s]\n",__FILE__,__LINE__,payin.F_payactno);
    vtcp_log("[%s][%d] cashactno[%s]\n",__FILE__,__LINE__,payin.F_cashactno);
    int iOrderno=0;
    vtcp_log("[%s][%d] 准备生成支付交易序号\n",__FILE__,__LINE__);
    iOrderno=iLvGetOrderno();
    if(iOrderno<=0)
    {
        sprintf( acErrMsg,"得到支付交易序号出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
    /***根据输入内容生成一条新的纪录****/
    vtcp_log("[%s][%d] payin结构体F to T\n",__FILE__,__LINE__);
    g_reply_int = iLvNewRecFromPayin(&payin);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"生成一条新的纪录出错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /***插入新记录****/
    vtcp_log("[%s][%d] 插入登记薄及明细记录\n",__FILE__,__LINE__);
    g_reply_int = iLvUpdRecFromPayin(&payin,1);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"插入新记录,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    /*****包检查****/
    vtcp_log("[%s][%d]F_packid=[%s]\n",__FILE__,__LINE__,payin.F_packid);
    payin.F_packend[0]='1';    /**实时业务就一个包，直接就封包**/
    iRc=iLvPayinCheck8(&payin,&lv_wbctl,NULL);
    if(iRc!=PCHK_UPDATE && iRc!=PCHK_NEW)
    {
        vtcp_log("[%s][%d]======0新包,1已存在,2已更新===iRc=[%d]\n",__FILE__,__LINE__,iRc);
        sprintf( acErrMsg,"无法拼到指定包,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    memcpy(payin.T_packday,payin.A_tbsdy,sizeof(payin.T_packday));
    memcpy(payin.F_packday,payin.A_tbsdy,sizeof(payin.F_packday));
    vtcp_log("[%s][%d] pkgid====[%s]\n",__FILE__,__LINE__,payin.F_packid);
    vtcp_log("[%s][%d] packday==[%s]\n",__FILE__,__LINE__,payin.T_packday);
    g_reply_int = iLvUpdRecFromPayin(&payin,0);
    if(g_reply_int)
    {
        sprintf( acErrMsg,"iLvUpdRecFromPayin错,[%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01" );
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]更新payin结构体完成\n",__FILE__,__LINE__);
    /* 记流水日志 */
    g_reply_int = pub_ins_trace_log();
    if ( g_reply_int )
    {
        sprintf( acErrMsg, "记流水日志错" );
        strncpy(g_pub_tx.reply,"S023",4);
        WRITEMSG
        goto ErrExit;
    }
    /*****写交易明细表 ******/
    iRc=ilv10_PreTos();
    if(iRc)
    {
        sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
        strncpy(g_pub_tx.reply,"P079",4);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]程序处理完毕\n",__FILE__,__LINE__);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
    memcpy(g_pub_tx.reply,"T063",4);
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;

}
/******************************************************************************
*                                                                             *
*    FUNCTION:           lv10_PreTos()                                        *
*   DESCRIPTION:       为返回包赋值                                           *
******************************************************************************/
ilv10_PreTos()
{
    int iRc;
    /****构造输出内容payout2****/
    iLvSetPayout1(&payin);
    return(0);
}
