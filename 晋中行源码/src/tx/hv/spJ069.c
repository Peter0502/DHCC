/************************************************************
* �� �� ��:   spJ069.c
* ����������  ���ҵ��������ʴ���
*              ���ʱ�ΪRECEIVE_ACCT_QS '9' ״̬,�޸Ĳ���״̬����Ϊ���ܲ��޸�ֱ������
* ��    ��:   xyy
* ������ڣ�  2006-10-19 22:11
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
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
   /*** ��ʼ��ȫ�ֱ��� ***/
   pub_base_sysinit();
   memset(&hv_payin      , 0 , sizeof(hv_payin));
   memset(&hv_payin_modif, 0 , sizeof(hv_payin_modif));
    
   get_fd_data(DC_TX_BR_NO,cInBrno);
   get_fd_data("0050",cTxday);
   /***�õ����֧��ҵ��8583����***/
   g_reply_int = iHvTistoPayin(&hv_payin);
   if( g_reply_int )
   {
      sprintf( acErrMsg,"����iHvTistoPayin����,[%s][%d]", __FILE__,__LINE__);
       strcpy( g_pub_tx.reply, "HV01" );
       set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;;
   }
   vtcp_log("[%s][%d]����iHvTistoPayin ���",__FILE__,__LINE__);
   g_reply_int = FetchRecToPayin(&hv_payin);
   if(g_reply_int)
   {
       sprintf( acErrMsg,"����FetchRecToPayin����,[%s][%d]", __FILE__,__LINE__);
       strcpy( g_pub_tx.reply, "HV01" );
      set_zd_data(DC_GET_MSG,acErrMsg);
      WRITEMSG
      goto ErrExit;
   }
   /**����payin��Ϣ**/
   MEMCPY_DEBUG((char*)&hv_payin_modif, (char*)&hv_payin,sizeof(hv_payin_modif));
     
   /**����payin�ṹ����Ϣ����**/
   iHvResetPayin(&hv_payin);     
   /* ���ñ��Ƿ���Ҫ���д��ʴ��� */
   if(hv_payin.T_hv_sts[0]!=hv_payin.T_beg_sts[0])
   {
         vtcp_log("[%s][%d]���й���������stat=[%c],begstat=[%c]!\n",__FILE__,__LINE__,hv_payin.T_hv_sts[0],hv_payin.T_beg_sts[0]);
         sprintf( acErrMsg,"�ñ�ҵ���Ѿ����й���������,�����Դ��ʴ���!,[%s][%d]", __FILE__,__LINE__);
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
       sprintf( acErrMsg,"֧�й���,�������Ĳ��ܴ���[%s][%d]cInBrno==[%s]", __FILE__,__LINE__,cInBrno);
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
            vtcp_log("%s,%d,���Ǵ�������,״̬Ϊ=,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
             sprintf( acErrMsg,"�˱ʲ��Ǵ�������,[%s][%d]",__FILE__,__LINE__);
             strcpy( g_pub_tx.reply, "HV01" );
             set_zd_data(DC_GET_MSG,acErrMsg);
             WRITEMSG
             goto ErrExit; 
        }
        
   if(memcmp(hv_payin.F_opcd,OPCD_INACT_MODIFY,sizeof(hv_payin.F_opcd))==0)
   {
       
        /* add by LiuHuafeng 2006-10-20 19:30 ׼����ˮ�еĽ��׽��*/
        MEMCPY_DEBUG(cTxamt,hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt));
        zip_space(cTxamt);
        str2dbl(cTxamt,strlen(cTxamt),0,&g_pub_tx.tx_amt1);
        /** end by LiuHuafeng 2006-10-20 19:30 */

        MEMCPY_DEBUG(hv_payin.F_cash_ac_no,hv_payin_modif.F_cash_ac_no,sizeof(hv_payin.F_cash_ac_no));
        MEMCPY_DEBUG(hv_payin.F_cash_name ,hv_payin_modif.F_cash_name,sizeof(hv_payin.F_cash_name));
        MEMCPY_DEBUG(hv_payin.T_cash_ac_no,hv_payin_modif.F_cash_ac_no,sizeof(hv_payin.T_cash_ac_no));
        MEMCPY_DEBUG(hv_payin.T_cash_name ,hv_payin_modif.F_cash_name,sizeof(hv_payin.T_cash_name));
        MEMCPY_DEBUG(hv_payin.T_hv_brf   ,cInBrno,BRNO_LEN);/**���ʻ���**/
        MEMCPY_DEBUG(hv_payin.T_hv_brf+BRNO_LEN,cTxday,sizeof(cTxday)-1);/**����ʱ��**/

        get_zd_data("0040",hv_payin.T_send_tel_trace_no);/* ��¼��Ա����ˮ�ŷŵ�����ɣ�û�취��*/
        get_zd_data("0070",hv_payin.T_dealerr_tel);/* ����Ա*/
        memcpy(hv_payin.T_dealerr_date,cTxday,sizeof(cTxday)-1);
        vtcp_log("[%s][%d] T_trace_no====[%.6s]",__FILE__,__LINE__,hv_payin.T_trace_no);
        vtcp_log("[%s][%d] T_dealerr_tel=[%.6s]",__FILE__,__LINE__,hv_payin.T_dealerr_tel);
        vtcp_log("[%s][%d] T_dealerr_tel=[%.8s]",__FILE__,__LINE__,hv_payin.T_dealerr_date);
        g_reply_int = iHvUpdZfRecFromPayin(&hv_payin,0);
        if( g_reply_int )
        {
          vtcp_log("%s,%d,���µǼǲ�ʧ��,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
           sprintf( acErrMsg,"���µǼǲ�ʧ��,[%s][%d]",__FILE__,__LINE__);
           strcpy( g_pub_tx.reply, "HV01" );
           set_zd_data(DC_GET_MSG,acErrMsg);
           WRITEMSG
           goto ErrExit;
        }
        /* ����ˮ��־ */
        g_reply_int = pub_ins_trace_log();
        if ( g_reply_int )
        {
            sprintf( acErrMsg, "����ˮ��־��" );
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
       get_zd_data("0040",hv_payin.T_chk_trace_no);/* ������ˮ��*/
       get_zd_data("0070",hv_payin.T_chk);/* ���˲���Ա*/
       vtcp_log("[%s][%d] T_trace_no====[%.6s]",__FILE__,__LINE__,hv_payin.T_chk_trace_no);
       vtcp_log("[%s][%d] T_chk=[%.6s]",__FILE__,__LINE__,hv_payin.T_chk);
       memcpy(hv_payin.T_dealerr_date,cTxday,sizeof(cTxday)-1);
       vtcp_log("[%s][%d] T_dealerr_tel=[%.8s]",__FILE__,__LINE__,hv_payin.T_dealerr_date);
       g_reply_int=iHvUpdZfRecFromPayin(&hv_payin,0);
       if( g_reply_int )
       {
           vtcp_log("%s,%d,���µǼǲ�ʧ��,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
            sprintf( acErrMsg,"���µǼǲ�ʧ��,[%s][%d]",__FILE__,__LINE__);
            strcpy( g_pub_tx.reply, "HV01" );
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
       }
         /* ����ˮ��־ */
       g_reply_int = pub_ins_trace_log();
       if ( g_reply_int )
       {
           sprintf( acErrMsg, "����ˮ��־��" );
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
  sprintf(acErrMsg,"׼������ return: reply is[%s]\n",g_pub_tx.reply);
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
