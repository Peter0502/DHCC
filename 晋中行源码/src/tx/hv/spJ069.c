/************************************************************
* 文 件 名:   spJ069.c
* 功能描述：  大额业务错误来帐处理
*              记帐变为RECEIVE_ACCT_QS '9' 状态,修改不改状态，因为可能不修改直接入帐
* 作    者:   xyy
* 完成日期：  2006-10-19 22:11
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_define.h"
#include "hv_fd123.h"
#include "hv_zf_c.h"
#include "find_debug.h"

static HV_PAYIN_AREA hv_payin,hv_payin_modif;
extern void lv_fullspace_len(char * con,int len);
static char *Getywname(char *);
static void fullspace_len(char * con,int len);
static char cQsbrno[BRNO_LEN+1];
static char cInBrno[BRNO_LEN+1];
static char cTxday[9];
int spJ069()
{
     char cTxamt[17];/* add by LiuHuafeng 2006-10-20 19:28 */
   memset(cTxamt, 0 , sizeof(cTxamt));
   memset(cInBrno, 0 , sizeof(cInBrno));
   memset(cTxday , 0 , sizeof(cTxday));
   /*** 初始化全局变量 ***/
   pub_base_sysinit();
   memset(&hv_payin      , 0 , sizeof(hv_payin));
   memset(&hv_payin_modif, 0 , sizeof(hv_payin_modif));
    
   get_fd_data(DC_TX_BR_NO,cInBrno);
   get_fd_data("0050",cTxday);
   /***得到大额支付业务8583数据***/
   g_reply_int = iHvTistoPayin(&hv_payin);
   if( g_reply_int )
   {
      sprintf( acErrMsg,"调用iHvTistoPayin出错,[%s][%d]", __FILE__,__LINE__);
       strcpy( g_pub_tx.reply, "HV01" );
       set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;;
   }
   vtcp_log("[%s][%d]调用iHvTistoPayin 完成",__FILE__,__LINE__);
   g_reply_int = FetchRecToPayin(&hv_payin);
   if(g_reply_int)
   {
       sprintf( acErrMsg,"调用FetchRecToPayin出错,[%s][%d]", __FILE__,__LINE__);
       strcpy( g_pub_tx.reply, "HV01" );
      set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;
   }
   /**保存payin信息**/
   MEMCPY_DEBUG((char*)&hv_payin_modif, (char*)&hv_payin,sizeof(hv_payin_modif));
     
   /**根据payin结构体信息反填**/
   iHvResetPayin(&hv_payin);     
   /* 检查该笔是否需要进行错帐处理 */
   if(hv_payin.T_hv_sts[0]!=hv_payin.T_beg_sts[0])
   {
         vtcp_log("[%s][%d]进行过其他处理stat=[%c],begstat=[%c]!\n",__FILE__,__LINE__,hv_payin.T_hv_sts[0],hv_payin.T_beg_sts[0]);
         sprintf( acErrMsg,"该笔业务已经进行过其他处理,不可以错帐处理!,[%s][%d]", __FILE__,__LINE__);
         strcpy( g_pub_tx.reply, "P077" );
         set_zd_data(DC_GET_MSG,acErrMsg);
         WRITEMSG
         goto ErrExit;
   }
   /**
   if(hv_payin.T_hv_sts[0]!=RECEIVE_DATA_ERR)
   {
     if(memcmp(cInBrno,QS_BR_NO,BRNO_LEN)==0)
     {
       sprintf( acErrMsg,"支行挂帐,清算中心不能处理[%s][%d]cInBrno==[%s]", __FILE__,__LINE__,cInBrno);
       strcpy( g_pub_tx.reply, "S043" );
       WRITEMSG
       goto ErrExit;
     }
   }
   **/
   if( hv_payin.T_hv_sts[0] != RECEIVE_DATA_ERR &&hv_payin.T_hv_sts[0] != \
        RECEIVE_CUSTNAME_ERR && hv_payin.T_hv_sts[0] != RECEIVE_NMAC_ERR && \
          hv_payin.T_hv_sts[0] != RECEIVE_LMAC_ERR)
        {
            vtcp_log("%s,%d,不是错误来帐,状态为=,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
             sprintf( acErrMsg,"此笔不是错误来帐,[%s][%d]",__FILE__,__LINE__);
             strcpy( g_pub_tx.reply, "HV01" );
             set_zd_data(DC_GET_MSG,acErrMsg);
             WRITEMSG
             goto ErrExit; 
        }
        
   if(memcmp(hv_payin.F_opcd,OPCD_INACT_MODIFY,sizeof(hv_payin.F_opcd))==0)
   {
       
        /* add by LiuHuafeng 2006-10-20 19:30 准备流水中的交易金额*/
        MEMCPY_DEBUG(cTxamt,hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt));
        zip_space(cTxamt);
        str2dbl(cTxamt,strlen(cTxamt),0,&g_pub_tx.tx_amt1);
        /** end by LiuHuafeng 2006-10-20 19:30 */

        MEMCPY_DEBUG(hv_payin.F_cash_ac_no,hv_payin_modif.F_cash_ac_no,sizeof(hv_payin.F_cash_ac_no));
        MEMCPY_DEBUG(hv_payin.F_cash_name ,hv_payin_modif.F_cash_name,sizeof(hv_payin.F_cash_name));
        MEMCPY_DEBUG(hv_payin.T_cash_ac_no,hv_payin_modif.F_cash_ac_no,sizeof(hv_payin.T_cash_ac_no));
        MEMCPY_DEBUG(hv_payin.T_cash_name ,hv_payin_modif.F_cash_name,sizeof(hv_payin.T_cash_name));
        MEMCPY_DEBUG(hv_payin.T_hv_brf   ,cInBrno,BRNO_LEN);/**入帐机构**/
        MEMCPY_DEBUG(hv_payin.T_hv_brf+BRNO_LEN,cTxday,sizeof(cTxday)-1);/**入帐时间**/

        get_zd_data("0040",hv_payin.T_send_tel_trace_no);/* 把录入员的流水号放到这里吧，没办法了*/
        get_zd_data("0070",hv_payin.T_dealerr_tel);/* 操作员*/
        memcpy(hv_payin.T_dealerr_date,cTxday,sizeof(cTxday)-1);
        vtcp_log("[%s][%d] T_trace_no====[%.6s]",__FILE__,__LINE__,hv_payin.T_trace_no);
        vtcp_log("[%s][%d] T_dealerr_tel=[%.6s]",__FILE__,__LINE__,hv_payin.T_dealerr_tel);
        vtcp_log("[%s][%d] T_dealerr_tel=[%.8s]",__FILE__,__LINE__,hv_payin.T_dealerr_date);
        g_reply_int = iHvUpdZfRecFromPayin(&hv_payin,0);
        if( g_reply_int )
        {
          vtcp_log("%s,%d,更新登记簿失败,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
           sprintf( acErrMsg,"更新登记簿失败,[%s][%d]",__FILE__,__LINE__);
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
        goto OkExit;
   }
   else if(memcmp(hv_payin.F_opcd,OPCD_INACT,sizeof(hv_payin.F_opcd))==0)
   {
       hv_payin.T_hv_sts[0]=RECEIVE_ACCT_QS;
       get_zd_data("0040",hv_payin.T_chk_trace_no);/* 复核流水号*/
       get_zd_data("0070",hv_payin.T_chk);/* 复核操作员*/
       vtcp_log("[%s][%d] T_trace_no====[%.6s]",__FILE__,__LINE__,hv_payin.T_chk_trace_no);
       vtcp_log("[%s][%d] T_chk=[%.6s]",__FILE__,__LINE__,hv_payin.T_chk);
       memcpy(hv_payin.T_dealerr_date,cTxday,sizeof(cTxday)-1);
       vtcp_log("[%s][%d] T_dealerr_tel=[%.8s]",__FILE__,__LINE__,hv_payin.T_dealerr_date);
       g_reply_int=iHvUpdZfRecFromPayin(&hv_payin,0);
       if( g_reply_int )
       {
           vtcp_log("%s,%d,更新登记簿失败,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"更新登记簿失败,[%s][%d]",__FILE__,__LINE__);
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
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);

  sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
}
