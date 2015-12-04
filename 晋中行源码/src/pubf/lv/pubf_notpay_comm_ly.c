#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#define EXTERN
/**
#include "mbfe_pub.h"
#include "public.h"
#include "hv_pub.h"
#include "hvnet.h"
#include "errlog.h"
***/
#include "hvpack.h"
#include "hv_addit_c.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "hv_zfhtr_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "lv_define.h"
#include "seqctl_c.h"
#include "hv_define.h"
#include "hvstat_c.h"
#include "com_sys_parm_c.h"
#include "hv_fzf_c.h"
#include "lv_sysctl_c.h"
char* pcLvGetDay();
extern char *pcLvGetDay_work();
int iHv_checkstat(char * cmtno,char * yw_type);
/************************************************************************* 
* �� �� ��: iLvUpdFzfRecFromPayin                  
* ���������� ���� ��֧���ṹ T ���� ���� hv_fzf    
* ��    ��:  ChenMing                             
* ������ڣ� 2006-8-9                             
* �޸ļ�¼��                                      
*   ��   ��:                                      
*   �� �� ��:                                     
*   �޸�����:                                      
*************************************************************************/ 
int iLvUpdFzfRecFromPayin(HV_NOTPAY_AREA *phv_notpayin)
{            

  int ret=0;
  struct hv_fzf_c hv_fzf;
  memset(&hv_fzf, 0x00, sizeof(hv_fzf));

  vtcp_log("iHvUpdFzfRecFromPayin() ��ʼ \n");
  memcpy(hv_fzf.cmtno,phv_notpayin->T_cmtno,sizeof(hv_fzf.cmtno)-1);
  memcpy(hv_fzf.or_br_no,phv_notpayin->T_or_br_no,sizeof(hv_fzf.or_br_no)-1);
  if ( strlen(phv_notpayin->T_wt_date) > 0 )  
    hv_fzf.wt_date = apatoi(phv_notpayin->T_wt_date,8);
  memcpy(hv_fzf.orderno,phv_notpayin->T_orderno,sizeof(hv_fzf.orderno)-1);


  vtcp_log("cmtno=[%s] or_br_no=[%s]\n wt_date=[%d] orderno=[%s]\n", \
    hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);

    ret = Hv_fzf_Dec_Upd(g_pub_tx.reply,"cmtno='%s' and or_br_no='%s' and wt_date='%d' and orderno='%s'", \
      hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);
    if (ret != 0)
    {
      sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
      WRITEMSG
        return ret;
    }
    ret = Hv_fzf_Fet_Upd(&hv_fzf, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
      sprintf(acErrMsg,"��ѯ��֧�����ױ����!![%d]",ret);
      WRITEMSG
        return ret;
    }
    else if (ret == 100)
    {
      sprintf(acErrMsg,"��֧�����ױ��в����ڸü�¼!!");
      WRITEMSG
        strcpy(g_pub_tx.reply,"O127");
      return ret;
    }    
    iHvNotPayin_Copy(phv_notpayin,&hv_fzf,hv_fzf.reqtype[0],hv_fzf.o_packid,hv_fzf.o_pack_date);

    ret = Hv_fzf_Upd_Upd(hv_fzf, g_pub_tx.reply);
    if (ret != 0)
    {
      sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
      WRITEMSG
        return ret;
    }    
    Hv_fzf_Clo_Upd();  

  
}
  

/***********************************************************************     
* �� �� ��: iLvNotPayin_Copy                    
* ��������: ��HV_NOTPAY_AREA����Ӧ�ֶ�ֵ����hv_fzf_c����Ӧ�ֶ� 
* ��    ��:  ChenMing                            
* ������ڣ� 2006-8-9                            
* �޸ļ�¼�� liuyue ������ԭ������Լ�ԭ��ID�ֶ�                                    
*   ��   ��:                                      
*   �� �� ��:                                    
*   �޸�����:                                    
*************************************************************************/  
int iLvNotPayin_Copy(HV_NOTPAY_AREA *phv_notpayin, struct hv_fzf_c *hv_fzf, LV_NOTPAY_ADD *lv_add)
{
  memcpy(hv_fzf->cmtno,phv_notpayin->T_cmtno,sizeof(hv_fzf->cmtno)-1);
  
  memcpy(hv_fzf->or_br_no,phv_notpayin->T_or_br_no,sizeof(hv_fzf->or_br_no)-1);
  if ( strlen(phv_notpayin->T_wt_date) > 0 )  
    hv_fzf->wt_date = apatoi(phv_notpayin->T_wt_date,8);
  memcpy(hv_fzf->orderno,phv_notpayin->T_orderno,sizeof(hv_fzf->orderno)-1);
  
    vtcp_log(" phv_notpayin->T_otype[%s] ",phv_notpayin->T_otype);    
    hv_fzf->otype[0] = '2';/****С��ҵ��***/
  
  if( strlen(phv_notpayin->T_tx_date) > 0 )        hv_fzf->tx_date = apatoi(phv_notpayin->T_tx_date,8);
  memcpy(hv_fzf->br_no,phv_notpayin->T_br_no,BRNO_LEN);      
  vtcp_log("[%s] [%d] T_hv_fzf_sts = [%c]",__FILE__,__LINE__,phv_notpayin->T_hv_fzf_sts[0]); 
  hv_fzf->hv_fzf_sts[0] = phv_notpayin->T_hv_fzf_sts[0];    
  vtcp_log("[%s] [%d]hv_fzf->hv_fzf_sts = [%c]",__FILE__,__LINE__,hv_fzf->hv_fzf_sts[0]); 
  
  if ( strlen(phv_notpayin->T_tx_time) > 0 )       hv_fzf->tx_time = apatoi(phv_notpayin->T_tx_time,6);
    if ( strlen(phv_notpayin->T_trace_no) > 0 )     hv_fzf->trace_no = apatoi(phv_notpayin->T_trace_no,6);

  memcpy(hv_fzf->pay_qs_no,phv_notpayin->T_pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);
  memcpy(hv_fzf->ac_br_no,phv_notpayin->T_ac_br_no,sizeof(hv_fzf->ac_br_no)-1);        
  memcpy(hv_fzf->cash_qs_no,phv_notpayin->T_cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);

  vtcp_log("phv_notpayin->T_o_wt_date=[%s]   phv_notpayin->T_o_wt_date=[%s]",phv_notpayin->T_o_wt_date,phv_notpayin->T_o_wt_date);  
  hv_fzf->o_tx_date = apatoi(phv_notpayin->T_o_wt_date,8);   /* ���ڴ���ʱ��֤�κ�ʱ��o_wt_date��o_tx_dateһ�� */
  hv_fzf->o_wt_date = apatoi(phv_notpayin->T_o_wt_date,8);   /* ���ڴ���ʱ��֤�κ�ʱ��o_wt_date��o_tx_dateһ�� */

  memcpy(hv_fzf->o_or_br_no,phv_notpayin->T_o_or_br_no,sizeof(hv_fzf->o_or_br_no)-1);  
  memcpy(hv_fzf->o_ac_br_no,phv_notpayin->T_o_ac_br_no,sizeof(hv_fzf->o_ac_br_no)-1);
  vtcp_log("[%s] [%d]hv_fzf->o_tx_date = [%d]",__FILE__,__LINE__,hv_fzf->o_tx_date);

  memcpy(hv_fzf->tel,phv_notpayin->T_tel,sizeof(hv_fzf->tel)-1);
  memcpy(hv_fzf->send_tel,phv_notpayin->T_send_tel,sizeof(hv_fzf->send_tel)-1);
    
  memcpy(hv_fzf->o_orderno,phv_notpayin->T_o_payin_orderno,sizeof(hv_fzf->o_orderno)-1);
  memcpy(hv_fzf->o_cur_ind,phv_notpayin->T_o_cur_no,sizeof(hv_fzf->o_cur_ind)-1);
  vtcp_log("%s,%d,Gethere,T_AMT=%s",__FILE__,__LINE__,phv_notpayin->T_o_tx_amt);

   if(strlen(phv_notpayin->T_o_tx_amt) >0 )      
    dot_str2dbl(phv_notpayin->T_o_tx_amt,sizeof(phv_notpayin->T_o_tx_amt), &hv_fzf->o_tx_amt);
  vtcp_log("%s,%d,Gethere,D_AMT=%f",__FILE__,__LINE__,hv_fzf->o_tx_amt);
        
  memcpy(hv_fzf->o_cmtno,phv_notpayin->T_o_cmtno,sizeof(hv_fzf->o_cmtno)-1);
  hv_fzf->o_tx_type[0] = phv_notpayin->T_o_tx_type[0];
  memcpy(hv_fzf->sendco,phv_notpayin->T_sendco,sizeof(hv_fzf->sendco)-1);
  memcpy(hv_fzf->receco,phv_notpayin->T_receco,sizeof(hv_fzf->receco)-1);
  vtcp_log("[%s] [%d]hv_fzf->o_tx_type[0] = [%c]",__FILE__,__LINE__,hv_fzf->o_tx_type[0]);
  hv_fzf->req_date = apatoi(phv_notpayin->T_req_date,8);
  vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
  memcpy(hv_fzf->req_br_no,phv_notpayin->T_req_br_no,sizeof(hv_fzf->req_br_no)-1);
  vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
  memcpy(hv_fzf->req_content,phv_notpayin->T_req_content,sizeof(hv_fzf->req_content)-1);
  memcpy(hv_fzf->req_orderno,phv_notpayin->T_req_orderno,sizeof(hv_fzf->req_orderno)-1);  
  hv_fzf->res_date = apatoi(phv_notpayin->T_res_date,8);
  vtcp_log("[%s] [%d]hv_fzf->res_date = [%d]",__FILE__,__LINE__,hv_fzf->res_date);
  memcpy(hv_fzf->res_br_no,phv_notpayin->T_res_br_no,sizeof(hv_fzf->res_br_no)-1);
  memcpy(hv_fzf->res_content,phv_notpayin->T_res_content,sizeof(hv_fzf->res_content)-1);
  memcpy(hv_fzf->res_orderno,phv_notpayin->T_res_orderno,sizeof(hv_fzf->res_orderno)-1);
  
  hv_fzf->res_sts[0] = phv_notpayin->T_respcode[0];
  vtcp_log("[%s] [%d]hv_fzf->res_sts = [%c]",__FILE__,__LINE__,hv_fzf->res_sts[0]);
  memcpy(hv_fzf->addid,phv_notpayin->T_addid,sizeof(hv_fzf->addid)-1);
  memcpy(hv_fzf->refno,phv_notpayin->T_refno,sizeof(hv_fzf->refno)-1);
   
  memcpy(hv_fzf->hv_prt_ind,phv_notpayin->T_hv_prt_ind,sizeof(hv_fzf->hv_prt_ind)-1);
  memcpy(hv_fzf->hv_brf,phv_notpayin->T_hv_brf,sizeof(hv_fzf->hv_brf)-1);
  hv_fzf->send_trace_no = apatoi(phv_notpayin->T_o_trace_no,6);
  vtcp_log("[%s] [%d]hv_fzf->o_trace_no= [%d]",__FILE__,__LINE__,hv_fzf->send_trace_no);
  vtcp_log("[%s] [%d]hv_fzf->refno= [%s]",__FILE__,__LINE__,hv_fzf->refno);
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  hv_fzf->lw_ind[0] = phv_notpayin->T_lw_ind[0];/* ������ʶ 1-�� 2-��  */
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__); 


  /****�������С���Ҫ�������漸���ֶ�*******/
  vtcp_log("[%s][%d]funk!\n",__FILE__,__LINE__);
  vtcp_log("[%s][%d] hv_fzf->reqtype[0]=[%c]  lv_add->reqtype[0]=[%c]",__FILE__,__LINE__,hv_fzf->reqtype[0],lv_add->reqtype[0]);  
  hv_fzf->reqtype[0] = lv_add->reqtype[0];/***���ʻ������������****/
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  
  memcpy(hv_fzf->o_packid,lv_add->opackid,sizeof(hv_fzf->o_packid)-1);/**ԭ������***/
  vtcp_log("[%s][%d]hv_fzf->o_packid===[%s]\n",__FILE__,__LINE__,hv_fzf->o_packid);
  vtcp_log("[%s][%d]lv_add->opack_date==[%s]\n",__FILE__,__LINE__,lv_add->opack_date);
  hv_fzf->o_pack_date = apatoi(lv_add->opack_date,sizeof(lv_add->opack_date));
  vtcp_log("[%s][%d]hv_fzf.o_pack_date==[%ld]\n",__FILE__,__LINE__,hv_fzf->o_pack_date);
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  memcpy(hv_fzf->otxnum,lv_add->otxnum,sizeof(hv_fzf->otxnum)-1);/***ԭҵ����***/
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);  
    /* */
    vtcp_log("[%s][%d]ִ�к���iLvNotPayin_Copy �ɹ�����!\n",__FILE__,__LINE__);
    return 0;  
}
/***********************************************************************     
* �� �� ��: iLvFzfToFzfhtr                    
* ��������: ��hv_fzf����Ӧ�ֶ�ֵ����hv_fzfhtr
* ��    ��:  liuyue                            
* ������ڣ� 2006-9-12                                                             
*************************************************************************/ 
int iLvFzfToFzfhtr(struct hv_fzf_c *hv_fzf,struct hv_fzfhtr_c *hv_fzfhtr)
{
  vtcp_log("[%s][%d]��ʼ���д�hv_fzf��hv_fzfhtr�ĸ���!\n",__FILE__,__LINE__);
  
  memcpy(hv_fzfhtr->or_br_no,hv_fzf->or_br_no,sizeof(hv_fzfhtr->or_br_no)-1);
  memcpy(hv_fzfhtr->orderno,hv_fzf->orderno,sizeof(hv_fzfhtr->orderno)-1);
  hv_fzfhtr->tx_date = hv_fzf->tx_date;
  hv_fzfhtr->wt_date = hv_fzf->wt_date;
  memcpy(hv_fzfhtr->br_no,hv_fzf->br_no,sizeof(hv_fzfhtr->br_no)-1);
  hv_fzfhtr->hv_fzfhtr_sts[0] = hv_fzf->hv_fzf_sts[0];
  memcpy(hv_fzfhtr->cmtno,hv_fzf->cmtno,sizeof(hv_fzfhtr->cmtno)-1);
  hv_fzfhtr->tx_time = hv_fzf->tx_time;
  hv_fzfhtr->trace_no = hv_fzf->trace_no;
  memcpy(hv_fzfhtr->pay_qs_no,hv_fzf->pay_qs_no,sizeof(hv_fzfhtr->pay_qs_no)-1);
  memcpy(hv_fzfhtr->ac_br_no,hv_fzf->ac_br_no,sizeof(hv_fzfhtr->ac_br_no)-1);
  memcpy(hv_fzfhtr->cash_qs_no,hv_fzf->cash_qs_no,sizeof(hv_fzfhtr->cash_qs_no)-1);
  hv_fzfhtr->o_tx_date = hv_fzf->o_tx_date;
  memcpy(hv_fzfhtr->o_or_br_no,hv_fzf->o_or_br_no,sizeof(hv_fzfhtr->o_or_br_no)-1);
  memcpy(hv_fzfhtr->o_ac_br_no,hv_fzf->o_ac_br_no,sizeof(hv_fzfhtr->o_ac_br_no)-1);
  memcpy(hv_fzfhtr->o_orderno,hv_fzf->o_orderno,sizeof(hv_fzfhtr->o_orderno)-1);
  memcpy(hv_fzfhtr->o_cur_ind,hv_fzf->o_cur_ind,sizeof(hv_fzfhtr->o_cur_ind)-1);
  hv_fzfhtr->o_tx_amt = hv_fzf->o_tx_amt;
  memcpy(hv_fzfhtr->o_cmtno,hv_fzf->o_cmtno,sizeof(hv_fzfhtr->o_cmtno)-1);
  hv_fzfhtr->o_tx_type[0] = hv_fzf->o_tx_type[0];
  memcpy(hv_fzfhtr->sendco ,hv_fzf->sendco ,sizeof(hv_fzfhtr->sendco )-1);
  memcpy(hv_fzfhtr->receco,hv_fzf->receco,sizeof(hv_fzfhtr->receco)-1);
  hv_fzfhtr->req_date = hv_fzf->req_date;
  memcpy(hv_fzfhtr->req_br_no,hv_fzf->req_br_no,sizeof(hv_fzfhtr->req_br_no)-1);
  memcpy(hv_fzfhtr->req_content,hv_fzf->req_content,sizeof(hv_fzfhtr->req_content)-1);
  memcpy(hv_fzfhtr->req_orderno,hv_fzf->req_orderno,sizeof(hv_fzfhtr->req_orderno)-1);
  hv_fzfhtr->res_date = hv_fzf->res_date;
  memcpy(hv_fzfhtr->res_br_no,hv_fzf->res_br_no,sizeof(hv_fzfhtr->res_br_no)-1);
  memcpy(hv_fzfhtr->res_content,hv_fzf->res_content,sizeof(hv_fzfhtr->res_content)-1);
  memcpy(hv_fzfhtr->res_orderno,hv_fzf->res_orderno,sizeof(hv_fzfhtr->res_orderno)-1);
  hv_fzfhtr->res_sts[0] = hv_fzf->res_sts[0];
  memcpy(hv_fzfhtr->addid,hv_fzf->addid,sizeof(hv_fzfhtr->addid)-1);
  memcpy(hv_fzfhtr->refno,hv_fzf->refno,sizeof(hv_fzfhtr->refno)-1);
  memcpy(hv_fzfhtr->hv_prt_ind,hv_fzf->hv_prt_ind,sizeof(hv_fzfhtr->hv_prt_ind)-1);
  memcpy(hv_fzfhtr->hv_brf,hv_fzf->hv_brf,sizeof(hv_fzfhtr->hv_brf)-1);
  memcpy(hv_fzfhtr->tel,hv_fzf->tel,sizeof(hv_fzfhtr->tel)-1);
  hv_fzfhtr->lw_ind[0] = hv_fzf->lw_ind[0];
  vtcp_log("[%][%d]�������!\n",__FILE__,__LINE__);
  
  return 0;
}



/************************************************************************* 
* �� �� ��: iLvFzFCommonCheck                  
* �����������Է�֧�����׽���һЩ�����ļ�� (�����˴���֧���ṹ��)
* ��    ��:  liuyue                             
* ������ڣ� 2006-9-13                            
* �޸ļ�¼��                                      
* ��   ��:                                      
* �� �� ��:                                     
* �޸�����:                                      
*************************************************************************/ 

int iLvFzFCommonCheck(HV_NOTPAY_AREA  *hv_notpayin)
{
  int ret   =  0;
  
  char cOr_br_no[13];
  char cAc_br_no[13];
  char cPay_qsno[13];
  char cCash_qsno[13];
  char cSendco  [5];
  char cRececo  [5];
  char cWt_date  [9];
  char cCmtno  [4];
  int  iCmtno = 0;
  
  memset(cOr_br_no,0x00,sizeof(cOr_br_no));
  memset(cAc_br_no,0x00,sizeof(cAc_br_no));
  memset(cPay_qsno,0x00,sizeof(cPay_qsno));
  memset(cCash_qsno,0x00,sizeof(cCash_qsno));
  memset(cSendco,    0x00,sizeof(cSendco));
  memset(cRececo,   0x00,sizeof(cRececo));
  memset(cWt_date,  0x00,sizeof(cWt_date));
  memset(cCmtno,    0x00,sizeof(cCmtno));
   
  
  memcpy(cAc_br_no,hv_notpayin->F_ac_br_no,sizeof(cAc_br_no)-1);
  memcpy(cCmtno,   hv_notpayin->F_cmtno,sizeof(cCmtno)-1);
  
  zip_space(cCmtno);
  vtcp_log("[%s][%d]cmtno==[%s]\n",__FILE__,__LINE__,cCmtno);
  
  
  /*****���CMT����Ƿ�Ϸ�******/
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno�Ƿ�!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P065");
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]�Ƿ���CMTNO��!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P065");
    return (-1);
  }
  
  
  /******����Ƿ���ڷǷ��ַ�*****/
  iCmtno = apatoi(cCmtno,sizeof(cCmtno)-1);
  switch(iCmtno)
  {
    case 301:
    case 303:
    case 319:
    case 321:
    case 323:
    case 325:
    case 327:
      
    case 302:
    case 320:
    case 322:
    case 324:
    case 328:
         
           
         ret = iCharacterType(hv_notpayin->F_content,sizeof(hv_notpayin->F_content),'g');
         if(ret)
         {
           vtcp_log("[%s][%d]����ĸ����д��ڷǷ�����!������������!\n",__FILE__,__LINE__);
           vtcp_log("[%s][%d]content=[%s]\n",__FILE__,__LINE__,hv_notpayin->F_content);
           strcpy(g_pub_tx.reply,"P070");
           return (-1);
         }
         break;
  }
  
  
  /*****���Ӫҵ״̬*******/
  ret = iLv_checkstat(cCmtno,"");
  if(ret)
  {
    vtcp_log("[%s][%d]Ӫҵ״̬������!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P066");
    return (-1);
  }
  
  
  /********����к��Ƿ�Ϸ�****/
  ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no,cOr_br_no);
  if(ret)
  {
    vtcp_log("[%s][%d]ͨ�������Ų��ҷ����д���!\n",__FILE__,__LINE__);
    return (-1);
  }
  ret = iGet_full_hv_no(cOr_br_no, cSendco,cPay_qsno);
  if(ret)
  {
    vtcp_log("[%s][%d]ͨ���кŲ��ҷ�������,���������д���\n",__FILE__,__LINE__);
    return (-1);
  }
  zip_space(cAc_br_no);
  if(strlen(cAc_br_no)==12)/***�������ɸ�ʽ��ʱ�������λ4λ�ձ�����***/
  {
    ret = iGet_full_hv_no(cAc_br_no,cRececo,cCash_qsno);
    if(ret)
    {
      vtcp_log("[%s][%d]ͨ�����������ձ�����,���������д���!\n",__FILE__,__LINE__);
      return (-1);
    }
    /** NEWYX by LiuHuafeng 2007-4-4 6:09 ������
    if(!memcmp(cOr_br_no,cAc_br_no,sizeof(cOr_br_no)-1))
    {
      vtcp_log("[%s][%d]�����кͽ�����,������ͬһ��!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"P067");
      return (-1);
    }
    **************************************************/
    /*** Ʊ��Ӱ����Ե�dongxy 20070328
    if(!memcmp(cPay_qsno,cCash_qsno,sizeof(cPay_qsno)-1))
    {
      vtcp_log("[%s][%d]���������кͽ��������в�����ͬһ��!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"P068");
      return (-1);
    }
    ***/
  }
  
  /********�ж�һ�������Ƿ�Ϸ�
  memcpy(cWt_date,hv_notpayin->F_o_wt_date,sizeof(hv_notpayin->F_o_wt_date));
  if(strlen(cWt_date)!=0)
  {
    if(memcmp(cWt_date,hv_notpayin->F_wt_date,sizeof(hv_notpayin->F_wt_date))>0)
    {
      vtcp_log("ԭί�����ڲ��Ϸ�owtdate=[%.8s]\n",cWt_date);
      strcpy(g_pub_tx.reply,"P069");
      return (-1);
    }
  }
  *********/
  
  return 0;
  
}


/************************************************************************* 
* �� �� ��: iLvFzFCommonCheck2                  
* �����������Է�֧�����׽���һЩ�����ļ�� (������С��֧���ṹ��)
* ��    ��:  liuyue                             
* ������ڣ� 2006-9-18                            
* �޸ļ�¼��                                      
* ��   ��:                                      
* �� �� ��:                                     
* �޸�����:                                      
*************************************************************************/ 
int iLvFzFCommonCheck2(PAY_IN_AREA  *lv_payin)
{
  int ret   =  0;
  
  char cOr_br_no[13];
  char cAc_br_no[13];
  char cPay_qsno[13];
  char cCash_qsno[13];
  char cSendco  [5];
  char cRececo  [5];
  char cWt_date  [9];
  char cPkgno  [4];
  
  memset(cOr_br_no,0x00,sizeof(cOr_br_no));
  memset(cAc_br_no,0x00,sizeof(cAc_br_no));
  memset(cPay_qsno,0x00,sizeof(cPay_qsno));
  memset(cCash_qsno,0x00,sizeof(cCash_qsno));
  memset(cSendco,    0x00,sizeof(cSendco));
  memset(cRececo,   0x00,sizeof(cRececo));
  memset(cWt_date,  0x00,sizeof(cWt_date));
  memset(cPkgno,    0x00,sizeof(cPkgno));
  
  
  memcpy(cAc_br_no,lv_payin->F_acbrno,sizeof(cAc_br_no)-1);
  memcpy(cPkgno,   lv_payin->F_pkgno,sizeof(cPkgno)-1);
  
  zip_space(cPkgno);
  vtcp_log("[%s][%d]cPkgno==[%s]\n",__FILE__,__LINE__,cPkgno);
  
  if(strlen(cPkgno)!=3)
  {
    vtcp_log("[%s][%d]cmtno�Ƿ�!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cPkgno,"012",3)&&memcmp(cPkgno,"013",3))
  {
    vtcp_log("[%s][%d]�Ƿ���pkgno��!\n",__FILE__,__LINE__);
    return (-1);
  }
  
  ret = iLv_checkstat(cPkgno,"");
  if(ret)
  {
    vtcp_log("[%s][%d]Ӫҵ״̬������!\n",__FILE__,__LINE__);
    return (-1);
  }
  
  
  
  ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no,cOr_br_no);
  if(ret)
  {
    vtcp_log("[%s][%d]ͨ�������Ų��ҷ����д���!\n",__FILE__,__LINE__);
    return (-1);
  }
  vtcp_log("[%s][%d]cOr_br_no==[%.12s]\n",__FILE__,__LINE__,cOr_br_no);
  ret = iGet_full_hv_no(cOr_br_no, cSendco,cPay_qsno);
  if(ret)
  {
    vtcp_log("[%s][%d]ͨ���кŲ��ҷ�������,���������д���\n",__FILE__,__LINE__);
    return (-1);
  }
  zip_space(cAc_br_no);
  if(strlen(cAc_br_no)==12)/***�������ɸ�ʽ��ʱ�������λ4λ�ձ�����***/
  {
    ret = iGet_full_hv_no(cAc_br_no,cRececo,cCash_qsno);
    if(ret)
    {
      vtcp_log("[%s][%d]ͨ�����������ձ�����,���������д���!\n",__FILE__,__LINE__);
      return (-1);
    }
    if(!memcmp(cOr_br_no,cAc_br_no,sizeof(cOr_br_no)-1))
    {
      vtcp_log("[%s][%d]�����кͽ�����,������ͬһ��!\n",__FILE__,__LINE__);
      return (-1);
    }
    /**NEWYX rem by LiuHuafeng 20070403 23:59
    if(!memcmp(cPay_qsno,cCash_qsno,sizeof(cPay_qsno)-1))
    {
      vtcp_log("[%s][%d]���������кͽ��������в�����ͬһ��!\n",__FILE__,__LINE__);
      return (-1);
    }
    **NEWYX rem by LiuHuafeng 20070404 */
  }
  
  /********�ж�һ�������Ƿ�Ϸ�*********/
  memcpy(cWt_date,lv_payin->F_owtday,sizeof(lv_payin->F_owtday));
  if(strlen(cWt_date)!=0)
  {
    if(memcmp(cWt_date,lv_payin->F_wtday,sizeof(lv_payin->F_wtday))>0)
    {
      vtcp_log("ԭί�����ڲ��Ϸ�owtdate=[%.8s]\n",cWt_date);
      return (-1);
    }
  }
  
  return 0;
  
}
/***********************************************************************     
* �� �� ��: iLvJudge_Change_notpay_stat(strcut hv_fzf_c *hv_fzf)                    
* ��������: �жϷ�֧�����׼�¼��״̬�������ݽ��׸���״̬,���е�״̬����ȫ���ŵ�������
* ��    ��:  liuyue                            
* ������ڣ� 2006-9-12                                                             
*************************************************************************/ 
int iLvJudge_Change_notpay_stat(struct hv_fzf_c *hv_fzf,char *cmtno) 
{
  vtcp_log("[%s][%d]�ж�---�ı��֧�����׼�¼��״̬!\n",__FILE__,__LINE__);
  char cCmtno[4];
  int  iCmtno   ;
  memset(cCmtno,0x00,sizeof(cCmtno));
  
  memcpy(cCmtno,cmtno,sizeof(cCmtno)-1);
  
  vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,cCmtno);
  zip_space(cCmtno);
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno�Ƿ�!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]�Ƿ���CMTNO��!\n",__FILE__,__LINE__);
    return (-1);
  }
  vtcp_log("[%s][%d]ccmtno=[%.3s]\n",__FILE__,__LINE__,cCmtno);
  
  iCmtno = atoi(cCmtno);
  
  switch(iCmtno)
  {
    case 301:
    case 303:
    case 319:
    case 321:
    case 323:
    case 327:       /**������Ľ���*****/
        hv_fzf->hv_fzf_sts[0] = STAT_WZLR;
        break;
    case 302:
    case 320:
    case 322:
    case 324:
        vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
        
        if(hv_fzf->hv_fzf_sts[0]!=STAT_LZRECV1)
        {
          vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
          vtcp_log("��Ҫ״̬[%c]ʵ��״̬[%c]\n",STAT_LZRECV1,hv_fzf->hv_fzf_sts[0]);
          return (-1);
        }
        vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
        /*״̬�Ȳ��������****/
        /*hv_fzf->hv_fzf_sts[0] = STAT_LZBACK;*/
        break;
    default:
        vtcp_log("[%s][%d]û���κζ���!\n",__FILE__,__LINE__);
  }
  vtcp_log("[%s][%d] stat=[%c]\n",__FILE__,__LINE__,hv_fzf->hv_fzf_sts[0]);
  
  
  return 0;
}

/***********************************************************************     
* �� �� ��: iLvJudge_Change_payin_stat(strcut lv_pkgreg_c *lv_pkgreg��char *cmtno)                    
* ��������: �ж�С��֧�����׼�¼��״̬�������ݽ��׸���״̬,���е�״̬����ȫ���ŵ�������
* ��    ��:  liuyue                            
* ������ڣ� 2006-9-12                                                             
*************************************************************************/ 
int iLvJudge_Change_payin_stat(struct lv_pkgreg_c *lv_pkgreg,char *cmtno)  
{
  vtcp_log("[%s][%d]�ж�---�ı�֧�����׼�¼��״̬!\n",__FILE__,__LINE__);
  char cCmtno[4];
  int  iCmtno   ;
  memset(cCmtno,0x00,sizeof(cCmtno));
  
  memcpy(cCmtno,cmtno,3);
  
  vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,cCmtno);
  zip_space(cCmtno);
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno�Ƿ�!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]�Ƿ���CMTNO��!\n",__FILE__,__LINE__);
    return (-1);
  }
  vtcp_log("[%s][%d]ccmtno=[%.3s]\n",__FILE__,__LINE__,cCmtno);
  
  iCmtno = atoi(cCmtno);
  
  switch(iCmtno)
  {
    case 301:
    case 302:
    case 303:
    case 320:
    case 322:
    case 324:
    case 325:
    case 328:
        break;
    case 319:
        if(lv_pkgreg->lv_sts[0]!=STAT_WZSUCCESS&&lv_pkgreg->lv_sts[0]!=STAT_WZSEND)
        {
          vtcp_log("[%s][%d]�˻�����,ԭ֧�����׼�¼����!\n",__FILE__,__LINE__);
          vtcp_log("table=[%c] [%c][%c]\n",lv_pkgreg->lv_sts[0],STAT_WZSUCCESS,STAT_WZSEND);
          return (-1);
        }
        break;
    case 321:
        if(lv_pkgreg->lv_sts[0]!=STAT_WZINQUEUE&&lv_pkgreg->lv_sts[0]!=STAT_WZSEND)
        {
          vtcp_log("[%s][%d]�˻�����,ԭ֧�����׼�¼����!\n",__FILE__,__LINE__);
          vtcp_log("table=[%c] [%c][%c]\n",lv_pkgreg->lv_sts[0],STAT_WZINQUEUE,STAT_WZSEND);
          return (-1);
        }
        break;
    case 323:
    case 327:       /**������Ľ���*****/
        if(lv_pkgreg->lv_sts[0]!=STAT_WZSEND)
        {
          vtcp_log("[%s][%d]�˻�����,ԭ֧�����׼�¼����!\n",__FILE__,__LINE__);
          vtcp_log("table=[%c] [%c]\n",lv_pkgreg->lv_sts[0],STAT_WZSEND);
          return (-1);
        }
        break;
    default:
        vtcp_log("[%s][%d]û���κζ���!\n",__FILE__,__LINE__);
  }
  
  return 0;
}

/*************************************************************************/
/* �������ƣ�void lv_checkstat()                                         */
/*                                                                       */
/* ������������鵱ǰӪҵ״̬                                            */
/*                                                                       */
/* ���룺��                                                              */
/*                                                                       */
/* ����ֵ�� ��                                                           */
/* ˵����  ����TITA��鵱ǰ״̬�Ƿ��������ˣ���ҵ���������������*/
/*         it_txcom.txrsut�ʹ�����������������gs99��gs100                */
/*************************************************************************/
int iLv_checkstat(char * cmtno,char * yw_type){
  int ret=0;
  char cReply[5];
  struct lv_sysctl_c lv_sysctl;
  struct com_sys_parm_c com_sys_parm;

  char tmpBuf[3];
  memset(tmpBuf,'\0',sizeof(tmpBuf));

  memset(cReply, 0x00 , sizeof(cReply));
  memset(&lv_sysctl,0x00,sizeof(lv_sysctl));
  vtcp_log("%s,%d����������1===1,cmtno=[%s]\n",__FILE__,__LINE__,cmtno);
  
  ret=Lv_sysctl_Sel(cReply,&lv_sysctl,"1=1");
  if(ret!=0){
    vtcp_log("%s,%d,ȡ��С��Ӫҵ״̬����,sqlcode=%d\n",__FILE__,__LINE__,ret);
    strncpy(acErrMsg,"ȡ��С��Ӫҵ״̬����",40);
    set_zd_data("0130",acErrMsg);
    return(-1);
  }
  memset(&com_sys_parm, 0 , sizeof(com_sys_parm));
  ret=Com_sys_parm_Sel(cReply,&com_sys_parm,"1=1");
  if(ret!=0){
    vtcp_log("%s,%d,ȡ�ù���ϵͳ���������,sqlcode=%d\n",__FILE__,__LINE__,ret);
    strncpy(acErrMsg,"ȡ�ù���ϵͳ���������",40);
    return(-1);
  }

  if(com_sys_parm.sys_sts!=0)
  {
   if((com_sys_parm.sys_sts==1 || com_sys_parm.sys_sts==2) && memcmp(g_pub_tx.tx_code,"6388",4)==0)
    {
    vtcp_log("%s,%d,����С��ҵ�������ں���״̬Ϊ1��2��״̬�½���,tx_code=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
    }
    else
    {
    vtcp_log("%s,%d,��ǰ״̬���ɷ���С��ҵ��,sqlcode=%d\n",__FILE__,__LINE__,ret);
    strncpy(acErrMsg,"��ǰ״̬���ɷ���С��ҵ��",40);
    set_zd_data("0130",acErrMsg);
    return(-1);
    }
  }
  switch(lv_sysctl.stat[0]){
  case LV_STAT_PREPARE: 
    /*Ӫҵ׼��״̬,ֻ�ܽ��գ����ܷ���ҵ��*/
    if(memcmp(cmtno,"500",3))
    {
      vtcp_log("%s,%d,Ӫҵ׼��״̬���ܷ�����\n",__FILE__,__LINE__);
      strncpy(acErrMsg,"Ӫҵ׼��״̬���ܷ�����",40);
      return 1;
    }
    else
    {
      vtcp_log("%s,%d 500���Ĳ���״̬ ",__FILE__,__LINE__);
      return 0;
    }
  case LV_STAT_WORK:
    /*�ռ�Ӫҵ״̬*/
    vtcp_log("%s,%d,�ռ�Ӫҵ״̬�ܷ������н���\n",__FILE__,__LINE__);
    return 0;
  }
  return 0;
}
int iLvchkstat()
{
  int iRet=0;
  iRet=iLv_checkstat("","");
  if(iRet)
  {
    vtcp_log("%s,%d ��ǰϵͳ״̬����",__FILE__,__LINE__);
    return iRet;
  }
}
/*************************************************************************/
/* �������ƣ�int lv_check_notpay_record(HV_NOTPAY_AREA *hv_notpayin.....)*/
/*                                                                       */
/* ����������ΪӦ����Ľ��׼��ԭ�����ཻ���Ƿ����                      */
/*           ������ԭ��֧�����׵ļ�¼                                    */
/*                                                                       */
/* ����ֵ�� 0-�ɹ� 1-ʧ��                                                */
/*************************************************************************/
int  lv_check_notpay_record(HV_NOTPAY_AREA *hv_notpay,struct hv_fzf_c *hv_fzf,char *cmtno) 
{
  vtcp_log("[%s][%d]��ʼ���ԭ��֧�������Ƿ����!\n",__FILE__,__LINE__);
  
  char cCmtno[4];
  int  iCmtno   ;
  int  iOrderno;
  int  ret = 0;
  struct hv_fzf_c hv_fzf_1;
  
  
  memset(cCmtno, 0x00,sizeof(cCmtno));
  memset(&hv_fzf_1,0x00,sizeof(hv_fzf_1));
  
  
  vtcp_log("[%s][%d]hv_fzf1==[%s]\n",(char *)&hv_fzf_1);
  
  
  
  memcpy(hv_fzf->orderno,       hv_notpay->F_o_notpay_orderno,   sizeof(hv_fzf->orderno)-1);
        hv_fzf->wt_date = apatoi(hv_notpay->F_o_fzf_date,sizeof(hv_notpay->F_o_fzf_date));
        memcpy(hv_fzf->or_br_no,      hv_notpay->F_ac_br_no,           sizeof(hv_fzf->or_br_no)-1);

    
  memcpy(cCmtno,cmtno,3);
  
  vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,cCmtno);
  zip_space(cCmtno);
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno�Ƿ�!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]�Ƿ���CMTNO��!\n",__FILE__,__LINE__);
    return (-1);
  }
  vtcp_log("[%s][%d]ccmtno=[%.3s]\n",__FILE__,__LINE__,cCmtno);
  
  iCmtno = atoi(cCmtno);
  
  switch(iCmtno)
  {
    case 302:
    case 320:
    case 328:
      sprintf(hv_fzf->cmtno,"%3d",iCmtno-1);
      vtcp_log("[%s][%d]hv_fzf->cmtno==[%s]\n",__FILE__,__LINE__,hv_fzf->cmtno);
      ret = Hv_fzf_Dec_Upd(g_pub_tx.reply,"orderno='%s' and or_br_no='%s' and wt_date=%d and cmtno='%s' and otype='2' and lw_ind='2' ",
      hv_fzf->orderno,hv_fzf->or_br_no,hv_fzf->wt_date,hv_fzf->cmtno);
      if(ret)
      {
        vtcp_log("[%s][%d]���ҷ�֧�����׼�¼����!ret=[%d]\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P015");
        return (-1);
      }
      
      ret = Hv_fzf_Fet_Upd(hv_fzf,g_pub_tx.reply);
      if(ret&&ret!=100)
      {
        vtcp_log("[%s][%d]���ݿ��������\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P015");
        return (-1);
      }
      else if(ret==100)
      {
        vtcp_log("[%s][%d]ԭ��֧�����׼�¼������!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P017");
        return (-1);
      }
      
      /**�жϺ�����״̬*****/
      vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
      
      vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
      
      ret = iLvJudge_Change_notpay_stat(hv_fzf,cCmtno);
      if(ret)
      {
        vtcp_log("[%s][%d]�жϺ�����ԭ��֧����¼״̬����!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P052");
        return (-1);
      }
      vtcp_log("[%s][%d]���ɵ��µ�С�����ţ���%d��\n",__FILE__,__LINE__,iOrderno);
      /***��ȡ�������***/
      iOrderno=iLvGetFzfOrderno();
      if(iOrderno<=0)
      {
        sprintf( acErrMsg,"�õ�֧��������Ŵ���![%s][%d]",__FILE__,__LINE__);
        WRITEMSG
        return (-1);
      }
      vtcp_log("[%s][%d]���ɵ��µ�С�����ţ���%d��\n",__FILE__,__LINE__,iOrderno);
      sprintf(hv_notpay->F_orderno,"%08d",iOrderno);
      memcpy(hv_fzf->res_orderno,hv_notpay->F_orderno,sizeof(hv_fzf->res_orderno)-1);
      GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_fzf->res_br_no);
      vtcp_log("[%s][%d]res_br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->res_br_no);
      memcpy(hv_fzf->res_content,hv_notpay->F_content,sizeof(hv_fzf->res_content)-1);
      hv_fzf->res_date = apatoi(hv_notpay->F_wt_date,sizeof(hv_notpay->F_wt_date));
      vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
      hv_fzf->res_sts[0] = hv_notpay->F_respcode[0];
      ret = Hv_fzf_Upd_Upd(*hv_fzf,g_pub_tx.reply);
      vtcp_log("[%s][%d]\n",__FILE__,__LINE__); 
      if(ret)
      {
        vtcp_log("[%s][%d]����ԭ��֧�����׼�¼ʧ��!ret=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"P055");
        return (-1);
      }
    
    
      vtcp_log("[%s][%d]hv_fzf.res_orderno=[%s]\n",__FILE__,__LINE__,hv_fzf->res_orderno);
      vtcp_log("[%s][%d]hv_fzf.res_brno=[%s]\n",__FILE__,__LINE__,hv_fzf->res_br_no);
      vtcp_log("[%s][%d]hv_fzf.res_date=[%ld]\n",__FILE__,__LINE__,hv_fzf->res_date);

      
      vtcp_log("hv_fzf===[%s]\n",(char *)hv_fzf);
      Hv_fzf_Clo_Upd();
      vtcp_log("[%s][%d]��ԭ��֧�����׼�¼�Ĵ������!\n",__FILE__,__LINE__);
      break;
    
  }
  
  
  
  return (0);
}


/*************************************************************************/
/* �������ƣ�int  lv_check_payin_record(HV_NOTPAY_AREA *hv_notpayin,     */
/*             LV_NOTPAY_ADD *lv_addinfo,char *cmtno,char cReq_type)     */
/*                                                                       */
/* ����������������֧�����׶�Ӧ��֧�������Ƿ����                      */
/*           ���֧������״̬  ��Ҫ�͸���֧������״̬                    */
/*           �����¼���� ��ȡ �����ص���Ϣ��lv_addinfo��              */
/*           ���ǰ̨�����������,��֧����������ȡ������                 */
/*                                                                       */
/* ����ֵ�� 0-�ɹ� 1-ʧ��                                                */
/*************************************************************************/

int  lv_check_payin_record(HV_NOTPAY_AREA *hv_notpayin,LV_NOTPAY_ADD *lv_addinfo,char *cmtno,char cReq_type)
{
  vtcp_log("[%s][%d]��ʼ��С���֧�����׽��м��!\n",__FILE__,__LINE__);
  
  struct lv_pkgreg_c lv_pkgreg;
  char cCmtno[4];
  int  iCmtno = 0;
  int  ret = 0;
  int  iRecordNum = 0;
  char cOr_br_no[13];
  char cAc_br_no[13];
  char cPack_date[9];
  
  char cF_ac_br_no[13];/***��ȡnotpayin�е�F_ac_br_no******/
  
  
  int  iQsno = 0;/***��ǰ�������Ƿ����������ģ�1 �� 0 ���ǣ���������ʱ���õ�****/
  int  iAcbrno = 1;/**�ж��������Ƿ��Ǹ�ͬһ�����У��ǽ���hv_notpay->F_ac_br_no��ֵ***/
  /*����Ƿ�������е� ����F_ac_br_no��ֵΪ�������� 1 �������ʼ�¼ͬ������ 0 ��ͬ������**/
  int  iSame   = 1;/*0-���ڶ�������� 1 ���͸�һ��֧�е�***/
  
  memset(&lv_pkgreg,  0x00,  sizeof(lv_pkgreg));
  memset(cCmtno,    0x00,  sizeof(cCmtno));
  memset(cOr_br_no,  0x00,  sizeof(cOr_br_no));
  memset(cPack_date,  0x00,  sizeof(cPack_date));
  
  
  
  memcpy(cF_ac_br_no,hv_notpayin->F_ac_br_no,sizeof(cF_ac_br_no)-1);
  zip_space(cF_ac_br_no);
  
  GetOr_br_noByBr_no(g_pub_tx.tx_br_no,cOr_br_no);
  if(memcmp(cOr_br_no,LV_QSBRNO,12))
    iQsno = 0;
  else 
    iQsno = 1;
  vtcp_log("[%s][%d]��ǰ����0 ������������ 1 ����������  ��%d��\n",__FILE__,__LINE__,iQsno);
      
  memcpy(cCmtno,cmtno,3);
  iCmtno = atoi(cCmtno);
  
  vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,cCmtno);
  zip_space(cCmtno);
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno�Ƿ�!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]�Ƿ���CMTNO��!\n",__FILE__,__LINE__);
    return (-1);
  }
  vtcp_log("[%s][%d]ccmtno=[%.3s]\n",__FILE__,__LINE__,cCmtno);
  vtcp_log("[%s][%d]reqtype==[%c]\n",__FILE__,__LINE__,cReq_type);
  
  
  vtcp_log("[%s][%d]��ʼ���ģ�hv_notpayin.F_ac_br_no===[%.12s]\n",__FILE__,__LINE__,cF_ac_br_no);
  
  if(cReq_type=='0')
  {
    vtcp_log("[%s][%d]ǰ̨������ǵ��ʵĴ���!\n",__FILE__,__LINE__);
    memcpy(lv_pkgreg.orderno,hv_notpayin->F_o_payin_orderno,sizeof(lv_pkgreg.orderno)-1);
    memcpy(lv_pkgreg.or_br_no,hv_notpayin->F_or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
    lv_pkgreg.wt_date = apatoi(hv_notpayin->F_o_wt_date,sizeof(hv_notpayin->F_o_wt_date));
    
    ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"orderno='%.8s' and or_br_no='%.12s' and wt_date=%d ",
        lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date);
    if(ret)
    {
      vtcp_log("[%s][%d]���ݿ�������� !\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"D103");
      return (-1);
    }
    
    ret = Lv_pkgreg_Fet_Upd(&lv_pkgreg,g_pub_tx.reply);
    if(ret&&ret!=100)
    {
      vtcp_log("[%s][%d]�����α����!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"D101");
      return (-1);
    }
    else if(ret==100)
    {
      vtcp_log("[%][%d]��¼������!\n",__FILE__,__LINE__);
      if(iCmtno==301)
      {
        vtcp_log("[%s][%d]���ǲ�ѯ����,���������¼���������!\n",__FILE__,__LINE__);
        Lv_pkgreg_Clo_Upd();
        return 0;
      }
      else
      {
        vtcp_log("[%s][%d]ԭ֧�����׼�¼������!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P017");
        return (-1);
      }
    }
    vtcp_log("[%s][%d]������˵��ԭ֧�����׼�¼�Ǵ��ڵ�!\n",__FILE__,__LINE__);
    ret = iLvJudge_Change_payin_stat(&lv_pkgreg,cCmtno);
    if(ret)
    {
    vtcp_log("[%s][%d]����С��֧�����׼�¼����!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P057");
    return (-1);
    }
    
    ret = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
    if(ret)
    {
      vtcp_log("[%s][%d]����С��֧�����׼�¼����!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"P058");
      return (-1);
    }


    Lv_pkgreg_Clo_Upd();
    
    memcpy(lv_addinfo->opackid,lv_pkgreg.packid,sizeof(lv_addinfo->opackid));
    memcpy(lv_addinfo->otxnum,  lv_pkgreg.txnum,sizeof(lv_addinfo->otxnum));
    vtcp_log("[%s][%d]lv_addinfo.otxnum==[%.5s]\n",__FILE__,__LINE__,lv_addinfo->otxnum);
    
    sprintf(cPack_date,"%08d",lv_pkgreg.pack_date);
    memcpy(lv_addinfo->opack_date,cPack_date,sizeof(lv_addinfo->opack_date));
    vtcp_log("[%s][%d]lv_addinfo.otxnum==[%.5s]\n",__FILE__,__LINE__,lv_addinfo->otxnum);
    
    memcpy(lv_addinfo->opkgno,lv_pkgreg.pkgno,sizeof(lv_addinfo->opkgno));
    vtcp_log("[%s][%d]lv_addinfo.otxnum==[%.5s]\n",__FILE__,__LINE__,lv_addinfo->otxnum);
    
    lv_addinfo->reqtype[0]  = cReq_type;
    vtcp_log("[%s][%d]lv_addinfo.otxnum==[%.5s]\n",__FILE__,__LINE__,lv_addinfo->otxnum);
    
    vtcp_log("packid=[%s]txnum=[%s]pack_date=[%ld]pkgno=[%s]reqtype=[%c]\n",lv_pkgreg.packid,lv_pkgreg.txnum,lv_pkgreg.pack_date,lv_pkgreg.pkgno,cReq_type);
    vtcp_log("[%s][%d]lv_addinfo.opackid==[%.8s]\n",__FILE__,__LINE__,lv_addinfo->opackid);
    vtcp_log("[%s][%d]lv_addinfo.opack_date==[%.8s]\n",__FILE__,__LINE__,lv_addinfo->opack_date);
    vtcp_log("[%s][%d]lv_addinfo.otxnum==[%.5s]\n",__FILE__,__LINE__,lv_addinfo->otxnum);
    vtcp_log("[%s][%d]lv_addinfo.opkgno==[%.3s]\n",__FILE__,__LINE__,lv_addinfo->opkgno);
    vtcp_log("[%s][%d]lv_addinfo.reqtype==[%c]\n",__FILE__,__LINE__,lv_addinfo->reqtype[0]);
    
    vtcp_log("[%s][%d]lv_addinfo==[%s]\n",__FILE__,__LINE__,(char *)lv_addinfo);
    
    
    
    /**********���ڱ���319֮��Ľ���,�������Ҫ��֧���������ҵ�******/
    /*add by liuyue 20060925*******/
    if(cF_ac_br_no[0]=='\0')
    {  
      vtcp_log("[%s][%d]hv_notpayin->F_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpayin->T_cash_qs_no);
      /* �������к� */
      memcpy(hv_notpayin->F_ac_br_no  ,lv_pkgreg.ac_br_no   ,sizeof(hv_notpayin->F_ac_br_no));
      vtcp_log("[%s][%d]!!!!��֧����������ȡ�˽�����!\n",__FILE__,__LINE__);
      
    }
    /************add end******************/
    
    vtcp_log("[%s][%d]���ڵ��ʵ�����Ѿ��������!\n",__FILE__,__LINE__);  
  
  }
  else if(cReq_type=='1')
  {
    vtcp_log("[%s][%d]��ʼ������������Ľ���!\n",__FILE__,__LINE__);
    memcpy(lv_pkgreg.or_br_no,hv_notpayin->F_or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
    memcpy(lv_pkgreg.packid,hv_notpayin->F_o_payin_orderno,sizeof(lv_pkgreg.packid)-1);
    lv_pkgreg.pack_date = apatoi(hv_notpayin->F_o_wt_date,sizeof(hv_notpayin->F_o_wt_date));
    memcpy(lv_pkgreg.pkgno,hv_notpayin->F_o_cmtno,sizeof(lv_pkgreg.pkgno)-1);
    lv_pkgreg.lw_ind[0]=hv_notpayin->F_lw_ind[0];
    vtcp_log("[%s][%d]lv_pkgreg.lw_ind=[%c]\n",__FILE__,__LINE__,lv_pkgreg.lw_ind[0]);
    /****modify by xyy 2007-4-15 19:20 NEWYX****/
    ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"packid='%.8s' and pack_date=%d  and pkgno='%.3s' and lw_ind='%s' ",
      lv_pkgreg.packid,lv_pkgreg.pack_date,lv_pkgreg.pkgno,lv_pkgreg.lw_ind);
    
    if(ret)
    {
      vtcp_log("[%s][%d]���ݿ��������!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"D103");
      return (-1);
    }
    
    while(1)
    {
      ret = Lv_pkgreg_Fet_Upd(&lv_pkgreg,g_pub_tx.reply);
      vtcp_log("[%s][%d]���ǵڡ�%d���ʽ���\n",__FILE__,__LINE__,iRecordNum);
      if(ret&&ret!=100)
      {
        vtcp_log("[%s][%d]���ݿ��������!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"D101");
        return (-1);
      }
      
      else if(iRecordNum==0&&ret ==100)
      {
        vtcp_log("[%s][%d]ԭ֧�����׼�¼������!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P017");
        return (-1);
      }
      else if(iRecordNum&&ret==100)
      {
        vtcp_log("[%s][%d]�α��Ѿ�ȡ�������!\n",__FILE__,__LINE__);
        break;
      }
      
      
      /****����һ�ʼ�¼�Ľ����зŵ�cAc_br_no��****/
      if(iRecordNum==0)
      {
        memcpy(cAc_br_no,lv_pkgreg.ac_br_no,sizeof(lv_pkgreg.ac_br_no)-1);
      }
      vtcp_log("[%s][%d]�ڡ�%d���ʼ�¼��ac_br_no==[%s]\n",__FILE__,__LINE__,iRecordNum,lv_pkgreg.ac_br_no);
      
      if(iRecordNum&&!memcmp(cAc_br_no,lv_pkgreg.ac_br_no,sizeof(cAc_br_no)-1))
      {
        iAcbrno = 1;/***�����ʼ�¼�Ľ�������һ�µ�***/
        vtcp_log("[%s][%d]�����������ʼ�¼�Ľ�����һ��!\n",__FILE__,__LINE__);
      }
      else if(iRecordNum&&memcmp(cAc_br_no,lv_pkgreg.ac_br_no,sizeof(cAc_br_no)-1))
      {
        iAcbrno = 0;/***�����ʼ�¼�Ľ������ǲ�һ�µ�***/
        vtcp_log("[%s][%d]��������������¼�Ľ����в�һ��!\n",__FILE__,__LINE__);
      }
      /***���Ž���ǰ��¼��ac_br_no��ֵ��cAcbrno*****/
      memcpy(cAc_br_no,lv_pkgreg.ac_br_no,sizeof(lv_pkgreg.ac_br_no)-1);
      
      iSame  =  iSame*iAcbrno;
      
      /***��ʼ����ʱ��cOr_br_no���Ѿ�����˱��еĵ�12λ�к�******/
      /****�����������ҵ��������а���������ҵ��,���������������****/
      vtcp_log("[%s][%d]cOr_br_no=[%s] lv_pkgreg.or_br_no=[%s] orderno===[%.8s]\n",__FILE__,__LINE__,cOr_br_no,lv_pkgreg.or_br_no,lv_pkgreg.orderno);
      if(!iQsno&&memcmp(cOr_br_no,lv_pkgreg.or_br_no,sizeof(cOr_br_no)-1))
      {
        vtcp_log("[%s][%d]�������漰�������,ֻ���������������˽���!\n",__FILE__,__LINE__);
        vtcp_log("[%s][%d]������:[%s][%s]\n",__FILE__,__LINE__,cOr_br_no,lv_pkgreg.or_br_no);
        strcpy(g_pub_tx.reply,"P059");
        return (-1);
      }
      
      /***ȡ��ǰ��¼��or_br_no********/
      memset(cOr_br_no,0x00,sizeof(cOr_br_no));
      memcpy(cOr_br_no,lv_pkgreg.or_br_no,sizeof(cOr_br_no)-1);
      
      
      iRecordNum++;
      
      vtcp_log("[%s][%d]������������ĵڡ�%d����ҵ����!\n",__FILE__,__LINE__,iRecordNum);
      
      vtcp_log("[%s][%d]������˵��ԭ֧�����׼�¼�Ǵ��ڵ�!\n",__FILE__,__LINE__);
      ret = iLvJudge_Change_payin_stat(&lv_pkgreg,cCmtno);
      if(ret)
      {
        vtcp_log("[%s][%d]����С��֧�����׼�¼����!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P057");
        return (-1);
      }
    
      ret = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
      if(ret)
      {
        vtcp_log("[%s][%d]����С��֧�����׼�¼����!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P058");
        return (-1);
      }
      
    }
    
    
    if(iCmtno==323&&iRecordNum>1)
    {
      vtcp_log("[%s][%d]����������Ȼ��������,����������ֻ��һ��!\n");
      strcpy(g_pub_tx.reply,"P057");
      return (-1);
    }
    memcpy(lv_addinfo->opackid,lv_pkgreg.packid,sizeof(lv_pkgreg.packid)-1);
    memcpy(lv_addinfo->otxnum,  lv_pkgreg.txnum,sizeof(lv_pkgreg.txnum)-1);
    sprintf(cPack_date,"%08d",lv_pkgreg.pack_date);
    memcpy(lv_addinfo->opack_date,cPack_date,sizeof(lv_addinfo->opack_date));
    memcpy(lv_addinfo->opkgno,lv_pkgreg.pkgno,sizeof(lv_pkgreg.pkgno)-1);
    lv_addinfo->reqtype[0]  = cReq_type;
    vtcp_log("[%s][%d]lv_addinfo==[%s]\n",__FILE__,__LINE__,(char )lv_addinfo);
    
    Lv_pkgreg_Clo_Upd();
    
    /***�����˻�����Ƚ���,��������Ҫ��֧����¼���ҵ����****/
    
    
    vtcp_log("[%s][%d]iSame==[%d] 1-�������͸�ͬһ��  0-�������͸���ͬ����\n",__FILE__,__LINE__,iSame);
    if(iSame==1 && cF_ac_br_no[0]==0x00)/****��������Ƿ���һ��֧�е�***/
    {
      memcpy(hv_notpayin->F_ac_br_no,lv_pkgreg.ac_br_no,sizeof(lv_pkgreg.ac_br_no)-1);
      vtcp_log("[%s][%d]��ȡ�����е�F_ac_br_no==[%s]!\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
    }
    else if(iSame==0 && cF_ac_br_no[0]==0x00)/**���Ƿ����ü���֧�е�,������������**/
    {
      memcpy(hv_notpayin->F_ac_br_no,lv_pkgreg.cash_qs_no,sizeof(lv_pkgreg.cash_qs_no)-1);
      vtcp_log("[%s][%d]��ȡ�����������ĵ�������![%s]\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
    }
      
    vtcp_log("[%s][%d]hv_notpayin->F_ac_br_no==[%s]\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
    
    vtcp_log("[%s][%d]�������ʵ�����Ѿ��������!\n",__FILE__,__LINE__);
  }
  else
  {
    vtcp_log("[%s][%d]����ĵ���,��������!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P060");
    return (-1);
  }
  
  vtcp_log("[%s][%d]���»�ȡ��:hv_notpayin.F_ac_br_no===[%.12s]\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
  vtcp_log("[%s][%d]���»�ȡ��:hv_notpayin.F_ac_br_no===[%.12s]\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
  return 0;
  
}


/*************************************************************************
* �������ƣ�int  lv_Ins_Notpay_record(HV_NOTPAY_AREA *hv_notpayin,       *
*    struct hv_fzf_c *hv_fzf,LV_NOTPAY_ADD *lv_addinfo)       *
*                                                                        *
* ��������������ǰ̨��������ݲ���hv_fzf���¼ (�ǲ��벻�Ǹ�             *
*           ��hv_fzfhtr���в���һ����¼            *
* ע��    : ����֮ǰ��hv_fzf.cmtno�ֶθ�ֵ  ����!!!                      *
*                                                                        *
* ����ֵ�� 0-�ɹ� 1-ʧ��                                                 * 
**************************************************************************/
int lv_Ins_Notpay_record(HV_NOTPAY_AREA *hv_notpayin,struct hv_fzf_c *hv_fzf,LV_NOTPAY_ADD *lv_addinfo)
{
  vtcp_log("[%s][%d]��ʼ��hv_fzf/hv_fzfhtr���в����¼\n",__FILE__,__LINE__);
  /****������ڽ��׵Ĳ�ͬ��Ҫ�Բ�����ֶν��и��ģ������ķŵ����彻���иĶ� ���Ķ�hv_notpayin�Ϳ�����**/
  vtcp_log("[%s][%d]lv_addinfo==[%s]\n",__FILE__,__LINE__,(char )lv_addinfo);
  vtcp_log("[%s][%d]lv_addinfo.opackid==[%.8s]\n",__FILE__,__LINE__,lv_addinfo->opackid);
  vtcp_log("[%s][%d]lv_addinfo.opack_date==[%.8s]\n",__FILE__,__LINE__,lv_addinfo->opack_date);
  vtcp_log("[%s][%d]lv_addinfo.otxnum==[%.5s]\n",__FILE__,__LINE__,lv_addinfo->otxnum);
  vtcp_log("[%s][%d]lv_addinfo.opkgno==[%.2s]\n",__FILE__,__LINE__,lv_addinfo->opkgno);
  vtcp_log("[%s][%d]lv_addinfo.reqtype==[%c]\n",__FILE__,__LINE__,lv_addinfo->reqtype[0]);
  
  
  int ret      = 0;
  long iOrderno = 0;
  struct hv_fzfhtr_c hv_fzfhtr;
  
  memset(&hv_fzfhtr,0x00,sizeof(hv_fzfhtr));
  
  iOrderno=iLvGetFzfOrderno();
   if(iOrderno<=0)
  {
     sprintf( acErrMsg,"�õ�֧��������Ŵ���![%s][%d]",__FILE__,__LINE__);
     WRITEMSG
     return (-1);
  }
  vtcp_log("[%s][%d]iOrderno=[%d]\n",__FILE__,__LINE__,iOrderno);

  sprintf(hv_notpayin->F_orderno,"%08ld",iOrderno);
  

  vtcp_log("[%s][%d]��ʼ����F->T����!\n",__FILE__,__LINE__);
  ret = iLvfFrontToTable(hv_notpayin);
   if(ret)
   {
       sprintf(acErrMsg,"ִ�к���F->T����!\n");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P015");
   }
   vtcp_log("[%s][%d]hv_notpay->T_receco=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_receco);
   vtcp_log("[%s][%d]!!!!!req_orderno=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_req_orderno);
   vtcp_log("[%s][%d]!!!!!req_wtdate=[%s] \n",__FILE__,__LINE__,hv_notpayin->T_req_date);
   vtcp_log("[%s][%d]!!!!!req_br_no=[%s]  \n",__FILE__,__LINE__,hv_notpayin->T_req_br_no);
   vtcp_log("[%s][%d]!!!!!req_content=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_req_content);
   vtcp_log("[%s][%d]F->T���� �ɹ�!\n",__FILE__,__LINE__);
  
   vtcp_log("[%s][%d]��ʼ��hv_fzf�ṹ�帳ֵ!\n",__FILE__,__LINE__);
   ret = iLvNotPayin_Copy(hv_notpayin,hv_fzf,lv_addinfo);
   if(ret)
   {
           sprintf(acErrMsg,"ִ�к��� iLvNotPayin_Cop����!\n");
           WRITEMSG
           strcpy(g_pub_tx.reply,"HV01");
   }
   vtcp_log("[%s][%d]hv_notpay->T_receco=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_receco);
   vtcp_log("[%s][%d]!!!!!req_orderno=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_req_orderno);
   vtcp_log("[%s][%d]!!!!!req_wtdate=[%s] \n",__FILE__,__LINE__,hv_notpayin->T_req_date);
   vtcp_log("[%s][%d]!!!!!req_br_no=[%s]  \n",__FILE__,__LINE__,hv_notpayin->T_req_br_no);
   vtcp_log("[%s][%d]!!!!!req_content=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_req_content);
   vtcp_log("[%s][%d]lv_addinfo==[%s]\n",__FILE__,__LINE__,(char *)lv_addinfo);
   vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
   vtcp_log("[%s][%d]��hv_fzf�ṹ�帳ֵ �ɹ�!\n",__FILE__,__LINE__);
   
   vtcp_log("[%s][%d]��ʼ�����֧����!\n",__FILE__,__LINE__);
   /*****����һ��״̬******/
   ret = iLvJudge_Change_notpay_stat(hv_fzf,hv_fzf->cmtno);
   if(ret)
   {
     vtcp_log("[%s][%d]�жϺ�����ԭ��֧����¼״̬����!\n",__FILE__,__LINE__);
     strcpy(g_pub_tx.reply,"P052");
     return (-1);
   }
   
   vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
   vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
   vtcp_log("[%s][%d]hv_fzf.stat==[%c]\n",__FILE__,__LINE__,hv_fzf->hv_fzf_sts[0]);
   Hv_fzf_Debug(hv_fzf);
   vtcp_log("[%s][%d]��ӡ���!\n",__FILE__,__LINE__);
   ret = Hv_fzf_Ins(*hv_fzf,g_pub_tx.reply);
   if(ret)
   {
           sprintf(acErrMsg,"�������ݱ�hv_fzf����!ret==[%d]\n",ret);
           WRITEMSG
           strcpy(g_pub_tx.reply,"HV01");
           return (-1);
   }
   vtcp_log("[%s][%d]hv_fzf->res__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->res_br_no);
   vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
   vtcp_log("[%s][%d]hv_fzf->o_orderno==[%s]\n",__FILE__,__LINE__,hv_fzf->o_orderno);
   vtcp_log("[%s][%d]����hv_fzf�ɹ�!\n",__FILE__,__LINE__);
   
   vtcp_log("[%s][%d]��ʼ����hv_fzfhtr\n",__FILE__,__LINE__);

   iLvFzfToFzfhtr(hv_fzf,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/

   ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
   if(ret)
   {
           sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
           WRITEMSG
           strcpy(g_pub_tx.reply,"HV01");
           return (-1);
   }
   
   vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);
   
   return 0;
}



/****************************************************************************/
/* ��������: iLvfFrontToTable(HV_NOTPAY_AREA * hv_notpay)                   */
/* ��������: ��NOTPAY��F_�ֶθ�ֵ��T�ֶ�                                    */
/* ��������: 2006-8-19 19:12                                                */
/* ����:     xyy                                                            */
/****************************************************************************/
int iLvfFrontToTable(HV_NOTPAY_AREA * hv_notpay,char * cbuff)
{  
  int iRet=0;
  char cFilename[1000];
  char cBuff[2000];
  char cCmtno[4];
  FILE * fpr;
  char cOr_br_no[13];
  
  Gl_file_first=0;
  memset(cFilename, 0  , sizeof(cFilename));
  memset(cBuff,0,sizeof(cBuff));  
  memset(cCmtno,0,sizeof(cCmtno)-1);
  memset(cOr_br_no,0x00,sizeof(cOr_br_no));
  
  
  
  get_fd_data("0060",hv_notpay->T_tx_time);/* ����ʱ��*/
  get_fd_data("0040",hv_notpay->T_trace_no);/* ��ˮ��*/
  vtcp_log("[%s][%d]hv_notpay->T_tx_time=[%s]",__FILE__,__LINE__,hv_notpay->T_tx_time);
  vtcp_log("[%s][%d]hv_notpay->T_trace_no=[%s]",__FILE__,__LINE__,hv_notpay->T_trace_no);
  hv_notpay->T_otype[0] = hv_notpay->F_otype[0];/*0��� 1С��*/
  vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay->T_otype[0]);
  hv_notpay->T_lw_ind[0] = hv_notpay->F_lw_ind[0]; /* ������ʶ1-��2�� */
  hv_notpay->T_o_tx_type[0] = hv_notpay->F_o_tx_type[0];/* ԭ�������� */
  vtcp_log("[%s,%d]T_o_tx_type===[%c]\n",__FILE__,__LINE__,hv_notpay->T_o_tx_type[0]);
  hv_notpay->T_respcode[0] = hv_notpay->F_respcode[0];/* �˻�/����Ӧ��,0, ���� 1�������� */
  vtcp_log("[%s][%d]hv_notpay->F_respcode=[%c]",__FILE__,__LINE__,hv_notpay->F_respcode[0]);
  memcpy(hv_notpay->T_orderno,hv_notpay->F_orderno ,sizeof(hv_notpay->T_orderno));/*�������*/
  memcpy(hv_notpay->T_tx_date,hv_notpay->F_tx_date ,sizeof(hv_notpay->T_tx_date));/*��������*/
  memcpy(hv_notpay->T_wt_date,hv_notpay->F_wt_date ,sizeof(hv_notpay->T_wt_date));/*ί������*/
  memcpy(hv_notpay->T_br_no  ,hv_notpay->F_tx_br_no ,sizeof(hv_notpay->T_br_no));/*���׻���*/
  GetOr_br_noByBr_no(hv_notpay->T_br_no  ,hv_notpay->T_or_br_no);  /* �����к� */
  memcpy(cCmtno,hv_notpay->F_cmtno,sizeof(cCmtno)-1);
  memcpy(hv_notpay->T_cmtno ,hv_notpay->F_cmtno ,sizeof(hv_notpay->T_cmtno));/* �������� */
  vtcp_log("[%s][%d]hv_notpay->T_or_br_no=[%s]",__FILE__,__LINE__,hv_notpay->T_or_br_no);
  /*�õ��������ĺͷ�������������*/
  iGet_full_hv_no(hv_notpay->T_or_br_no,hv_notpay->T_sendco,hv_notpay->T_pay_qs_no);
  
  vtcp_log("[%s][%d]hv_notpay->T_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpay->T_cash_qs_no);
  /* ԭί������ */
  memcpy(hv_notpay->T_o_wt_date  ,hv_notpay->F_o_wt_date  ,sizeof(hv_notpay->T_o_wt_date ));
  /* ԭ�������к� */
  memcpy(hv_notpay->T_o_or_br_no ,hv_notpay->F_o_or_br_no ,sizeof(hv_notpay->T_o_or_br_no));
  /***Modified by SSH,2006.8.28,�������4λ��ʾ���ձ�����****/
  pub_base_full_anti_zero(hv_notpay->F_ac_br_no,sizeof(hv_notpay->F_ac_br_no));
  if(hv_notpay->F_ac_br_no[5]=='\0' && hv_notpay->F_ac_br_no[0]!=0x00)
  {  
    vtcp_log("[%s][%d] dhcc ���ǵ��ձ����ĵ����ɸ�ʽ!hv_notpay->F_ac_br_no==[%s]\n",__FILE__,__LINE__,hv_notpay->F_ac_br_no);
    memset(hv_notpay->T_ac_br_no  ,'\0',sizeof(hv_notpay->T_ac_br_no));/* �������к� */
    memcpy(hv_notpay->T_receco,hv_notpay->F_ac_br_no,sizeof(hv_notpay->T_receco));
    vtcp_log("[%s][%d]hv_notpay.T_receco===[%s]\n",__FILE__,__LINE__,hv_notpay->T_receco);
  }else
  {
    vtcp_log("[%s][%d]hv_notpay->T_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpay->T_cash_qs_no);
    /* �������к� */
    memcpy(hv_notpay->T_ac_br_no  ,hv_notpay->F_ac_br_no   ,sizeof(hv_notpay->T_ac_br_no));
    /*�õ������ĺͽ�������������*/
    iGet_full_hv_no(hv_notpay->T_ac_br_no,hv_notpay->T_receco,hv_notpay->T_cash_qs_no);
  }
  vtcp_log("[%s][%d]hv_notpay->T_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpay->T_cash_qs_no);
  /* ԭ�������к� */
  memcpy(hv_notpay->T_o_ac_br_no ,hv_notpay->F_o_ac_br_no ,sizeof(hv_notpay->T_o_ac_br_no));
  /* ԭ֧��������� */
  memcpy(hv_notpay->T_o_payin_orderno ,hv_notpay->F_o_payin_orderno ,sizeof(hv_notpay->T_o_payin_orderno ));
  /* ԭ���� */
  memcpy(hv_notpay->T_o_cur_no   ,hv_notpay->F_o_cur_no   ,sizeof(hv_notpay->T_o_cur_no));
  /* ԭ���׽�� */
  memcpy(hv_notpay->T_o_tx_amt   ,hv_notpay->F_o_txamt    ,sizeof(hv_notpay->T_o_tx_amt));
  /* ԭҵ������ */
  memcpy(hv_notpay->T_o_cmtno    ,hv_notpay->F_o_cmtno    ,sizeof(hv_notpay->T_o_cmtno));
  vtcp_log("[%s][%d]F_content=[%s]",__FILE__,__LINE__,hv_notpay->F_content);
  vtcp_log("[%s][%d]F_cmtno=[%s]",__FILE__,__LINE__,hv_notpay->F_cmtno);
  vtcp_log("[%s][%d]cCmtno=[%s]",__FILE__,__LINE__,cCmtno);
  /*****���������Ӧ����ֱ���*****//**by liuyue 20060913***/
  if(!(memcmp(cCmtno,"301",3)==0 ||memcmp(cCmtno,"303",3)==0||memcmp(cCmtno,"319",3)==0
    ||memcmp(cCmtno,"321",3)==0 ||memcmp(cCmtno,"323",3)==0||memcmp(cCmtno,"327",3)==0||memcmp(cCmtno,"302",3)==0 ||memcmp(cCmtno,"320",3)==0 ||memcmp(cCmtno,"322",3)==0 
    ||memcmp(cCmtno,"324",3)==0 ||memcmp(cCmtno,"328",3)==0 ))
  {
    vtcp_log("[%s][%d]�����CMTNO���!cmtno==[%.3s]\n",__FILE__,__LINE__,cCmtno);
    strcpy(g_pub_tx.reply,"P065");
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)==0 ||memcmp(cCmtno,"303",3)==0||memcmp(cCmtno,"319",3)==0
     ||memcmp(cCmtno,"321",3)==0 ||memcmp(cCmtno,"323",3)==0||memcmp(cCmtno,"327",3)==0)
  {
    /* ���뽻����� */
    memcpy(hv_notpay->T_req_orderno,hv_notpay->F_orderno,sizeof(hv_notpay->T_req_orderno));
    /* �������� */
    memcpy(hv_notpay->T_req_date,hv_notpay->F_wt_date ,sizeof(hv_notpay->T_req_date));
    /* ������� */
    vtcp_log("[%s][%d]hv_notpay->F_or_br_no=[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
    memcpy(hv_notpay->T_req_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_req_br_no)); 
    /* Ӧ����� */
    memcpy(hv_notpay->T_res_br_no,hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_res_br_no));
    vtcp_log("[%s][%d]hv_notpay->T_res_br_no=[%.12s]\n",__FILE__,__LINE__,hv_notpay->T_res_br_no);
    /* �������� */
    vtcp_log("[%s][%d]hv_notpay->T_req_br_no=[%.12s]\n",__FILE__,__LINE__,hv_notpay->T_req_br_no);
    memcpy(hv_notpay->T_req_content,hv_notpay->F_content,sizeof(hv_notpay->T_req_content));
    vtcp_log("[%s][%d]T_req_content=[%s]",__FILE__,__LINE__,hv_notpay->T_req_content);
}
  if(memcmp(cCmtno,"302",3)==0 ||memcmp(cCmtno,"320",3)==0 ||memcmp(cCmtno,"322",3)==0 
  ||memcmp(cCmtno,"324",3)==0 ||memcmp(cCmtno,"328",3)==0  )
  {  /* Ӧ������� */
    memcpy(hv_notpay->T_res_orderno,hv_notpay->F_orderno,sizeof(hv_notpay->T_res_orderno));
    /* Ӧ������ */
    memcpy(hv_notpay->T_res_date,hv_notpay->F_wt_date ,sizeof(hv_notpay->T_res_date));
    /* Ӧ����� */
    memcpy(hv_notpay->T_res_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_res_br_no));
    /* �������� */
    memcpy(hv_notpay->T_res_content,hv_notpay->F_content,sizeof(hv_notpay->T_res_content));
    vtcp_log("[%s][%d]T_res_content=[%s]",__FILE__,__LINE__,hv_notpay->T_res_content);
  }
  vtcp_log("[%s][%d]T_req_content=[%s]",__FILE__,__LINE__,hv_notpay->T_req_content);
  vtcp_log("[%s][%d]T_res_content=[%s]",__FILE__,__LINE__,hv_notpay->T_res_content);
  hv_notpay->T_o_tx_type[0] = hv_notpay->F_o_tx_type[0];/**ԭҵ������**/
  memcpy(hv_notpay->T_refno ,hv_notpay->F_refno  ,sizeof(hv_notpay->T_refno));/* ���Ĳο��� */
  memcpy(hv_notpay->T_tel    ,hv_notpay->F_tel    ,sizeof(hv_notpay->T_tel));/* ����Ա�� */
  memcpy(hv_notpay->T_send_tel,hv_notpay->F_send_tel,sizeof(hv_notpay->F_send_tel));
  memcpy(hv_notpay->T_hv_brf ,hv_notpay->F_hv_brf,sizeof(hv_notpay->T_hv_brf)); /* ��ע */
  
  

  if(memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
  {/* 01¼�� */
    hv_notpay->T_hv_fzf_sts[0]=SEND_REGIST;
  }
  else if(memcmp(hv_notpay->F_opcd,"30",sizeof(hv_notpay->F_opcd))==0)
  {/* 03���� */
    hv_notpay->T_hv_fzf_sts[0]=SEND_ACCT_WAIT;
  }
  else if(memcmp(hv_notpay->F_opcd,"60",sizeof(hv_notpay->F_opcd))==0)
  {/* 04���� */
    if (!memcmp(g_pub_tx.reply,"0000",4))
    {
      hv_notpay->T_hv_fzf_sts[0]=RECEIVE_ACCT_QS;
    }
    else
    {
      /* ����ʧ���ˣ�Ҫ���жϽ��ջ����Ƿ���ڣ����ڹ�֧�У������ڹ����� */
      hv_notpay->T_hv_fzf_sts[0]=RECEIVE_CUSTNAME_ERR;
    }
  }
  else if(memcmp(hv_notpay->F_opcd,"50",sizeof(hv_notpay->F_opcd))==0)
  {/* 05Ӧ�� */
    hv_notpay->T_hv_fzf_sts[0]=SEND_ACCT_QS;
  }
  else
  {
    vtcp_log("[%s][%d]��֧�����׺���û�д��ֽ������Ͱ���F_opcd=[%s]",__FILE__,__LINE__,hv_notpay->F_opcd);
    sprintf(acErrMsg,"��֧�����׺���û�д��ֽ������Ͱ���F_opcd!\n");
    WRITEMSG
    return 1;
  }
  vtcp_log("[%s][%d]cmtno=[%.3s]",__FILE__,__LINE__,hv_notpay->T_cmtno);

  
  
  vtcp_log("[%s][%d]F_opcd=[%s]",__FILE__,__LINE__,hv_notpay->F_opcd);
  vtcp_log("[%s][%d]fzf_sts=[%c]",__FILE__,__LINE__,hv_notpay->T_hv_fzf_sts[0]);
  return 0;
}


/****************************************************************************/
/* ��������: iLvTistoNotpay(HV_NOTPAY_AREA * hv_notpay)                     */
/* ��������: ��ǰ̨�õ����ݵ�F_�ֶ�                                         */
/* ��������: 2006-8-19 15:58                                                */
/* ����:     xyy                                                           */
/****************************************************************************/ 
int iLvTistoNotpay(HV_NOTPAY_AREA * hv_notpay)
{
  HV_FD123_AREA fd123;
  char cOrderno[9];
  char cInd[2];
  char cCmtno[4];
  char cCur_no[3];
  char cRececo[5];
  int iRet=0;
  char cOr_br_no[13];
  int iOrderno = 0;/*by liuyue 20060913*/
  
  
  memset(cCur_no, 0x00 , sizeof(cCur_no));
  memset(cInd, 0x00 , sizeof(cInd));
  memset(cRececo,0x00,sizeof(cRececo));
  memset(cOrderno, 0x00 , sizeof(cOrderno));
  memset(cCmtno,0x00,sizeof(cCmtno));
  memset(&fd123, 0x00 , sizeof(fd123));
  memset(cOr_br_no,0x00,sizeof(cOr_br_no));
  
  
  get_fd_data("1230",(char*)&fd123);
  
  vtcp_log("123��=[%s]",(char*)&fd123.ac_br_no); 
  
  vtcp_log("123��=[%s]",(char*)&fd123.cmtno); 
  
  memcpy(hv_notpay->F_cmtno,fd123.cmtno,sizeof(hv_notpay->F_cmtno));
  vtcp_log("ǰ̨������cmtno=[%.3s]",hv_notpay->F_cmtno); 
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  memcpy(hv_notpay->F_o_cmtno,fd123.o_cmtno,sizeof(hv_notpay->F_o_cmtno));
  vtcp_log("ǰ̨������o_cmtno=[%.3s]",hv_notpay->F_o_cmtno); 
  
  
  vtcp_log("ǰ̨������pay_qs_no ��Ҫ!!!\n");
  
  
  memcpy(hv_notpay->F_or_br_no,fd123.or_br_no,sizeof(hv_notpay->F_or_br_no));
  vtcp_log("ǰ̨������ ������/ԭ������ F_or_br_no=[%.12s]",hv_notpay->F_or_br_no); 
  
  
  
  vtcp_log("ǰ̨������sendco ��Ҫ!!!\n");
  vtcp_log("ǰ̨������pay_opn_br_no ��Ҫ!!!\n");
  vtcp_log("ǰ̨������pay_ac_no ��Ҫ!!!\n");
  vtcp_log("ǰ̨������  pay_name  ��Ҫ!!!\n");
  vtcp_log("ǰ̨������   pay_addr ��Ҫ!!!\n");
  vtcp_log("ǰ̨������  cash_qs_no ��Ҫ!!!\n");
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  memcpy(hv_notpay->F_ac_br_no,fd123.ac_br_no,sizeof(hv_notpay->F_ac_br_no));
  vtcp_log("ǰ̨������ ������ F_ac_br_no=[%.12s]",hv_notpay->F_ac_br_no); 
  
  
  vtcp_log("ǰ̨������  receco =[%.4s]!!!\n",fd123.receco);
  vtcp_log("ǰ̨������  cash_opn_br_no ��Ҫ!!!\n");
  vtcp_log("ǰ̨������  cash_ac_no ��Ҫ!!!\n");
  vtcp_log("ǰ̨������  cash_name ��Ҫ!!!\n");
  vtcp_log("ǰ̨������  cash_addr ��Ҫ!!!\n");
  vtcp_log("ǰ̨������  yw_type ��Ҫ!!!\n");
  
  
  memcpy(hv_notpay->F_orderno ,fd123.orderno ,sizeof(hv_notpay->F_orderno ));
  vtcp_log("ǰ̨������   ������� F_orderno =[%.8s]",hv_notpay->F_orderno); 
  
  
  memcpy(hv_notpay->F_hv_brf,fd123.brf,sizeof(hv_notpay->F_hv_brf));
  vtcp_log("ǰ̨������   ���� F_hv_brf =[%.60s]",hv_notpay->F_hv_brf); 
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  vtcp_log("ǰ̨������   ���� rate ��Ҫ!!!\n");
  vtcp_log("ǰ̨������   ��������ratelimit ��Ҫ!!!\n");
  
  
  memcpy(hv_notpay->F_o_payin_orderno ,fd123.o_orderno ,sizeof(hv_notpay->F_o_payin_orderno ));
  vtcp_log("ǰ̨������ ������� F_o_payin_orderno =[%.8s]",hv_notpay->F_o_payin_orderno); 
  
  
  memcpy(hv_notpay->F_resp1,fd123.procode,sizeof(hv_notpay->F_resp1));
  vtcp_log("ǰ̨������ ������� hv_notpay->F_resp1[%.8s]",hv_notpay->F_resp1);
  
  
  
  vtcp_log("ǰ̨������  �鸴����cr_date=[%.8s]!!!\n",fd123.cr_date);
  vtcp_log("ǰ̨������  �鸴�к�cr_br_no=[%.12s]!!!\n",fd123.cr_br_no);
  vtcp_log("ǰ̨������  �鸴���cr_tx_num =[%.8s]!!!\n",fd123.cr_tx_num );
  vtcp_log("ǰ̨������  ��ѯ�к�qr_br_no=[%.12s]!!!\n",fd123.qr_br_no);
  vtcp_log("ǰ̨������  ��ѯ���qr_tx_num=[%.8s]!!!\n",fd123.qr_tx_num);
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  hv_notpay->F_o_tx_type[0] = fd123.tx_type[0];
  vtcp_log("ǰ̨������  ԭҵ������ tx_type===[%c]\n",hv_notpay->F_o_tx_type[0]);
  
  
  memcpy(hv_notpay->F_content,fd123.notpay_content,sizeof(hv_notpay->F_content)); 
  vtcp_log("ǰ̨������  �������� F_content===[%.255s]\n",hv_notpay->F_content);
  
  /*
  memcpy(hv_notpay->F_o_notpay_orderno,fd123.notpay_orderno,sizeof(hv_notpay->F_o_notpay_orderno));
  vtcp_log("ǰ̨������  ��֧��������� F_o_notpay_orderno===[%.8s]\n",hv_notpay->F_o_notpay_orderno);*/
  
  /******************
  memcpy(hv_notpay->F_o_notpay_orderno,"00000",5);
  memcpy(hv_notpay->F_o_notpay_orderno+5,fd123.o_notpay_orderno,sizeof(fd123.o_notpay_orderno));
  vtcp_log("ǰ̨������  ԭ��֧��������� F_o_notpay_orderno===[%.8s]\n",hv_notpay->F_o_notpay_orderno);
  ************/
  memcpy(hv_notpay->F_o_notpay_orderno,fd123.qr_tx_num,sizeof(fd123.qr_tx_num));
  vtcp_log("ǰ̨������  ԭ��֧��������� F_o_notpay_orderno===[%.8s]\n",hv_notpay->F_o_notpay_orderno);
  
  
  hv_notpay->F_respcode[0] = fd123.resp[0];
  vtcp_log("ǰ̨������ �������˻�Ӧ�� hv_notpay->F_respcode[0]=[%c]",hv_notpay->F_respcode[0]); 
  
  
  vtcp_log("ǰ̨������  ���ȼ��� operlevel=[%c]!!!\n",fd123.operlevel[0]);
  
  
  vtcp_log("ǰ̨������  ����״̬ proc_sts=[%c]!!!\n",fd123.proc_sts[0]);
  
  
  memcpy(hv_notpay->F_opcd,fd123.opcd,sizeof(hv_notpay->F_opcd));
  vtcp_log("ǰ̨������ �������� opcd=[%.2s]",hv_notpay->F_opcd); 
   
  hv_notpay->F_lw_ind[0]=fd123.lw_ind[0];
  vtcp_log("ǰ̨������  ������־F_lw_ind[0]=[%c]",fd123.lw_ind[0]); 
  
  
  hv_notpay->F_hp_add_pwd[0] = fd123.hp_add_pwd[0];
  vtcp_log("ǰ̨������   ��Ʊ�Ƿ��Ѻ F_hp_add_pwd[0]=[%c]",hv_notpay->F_hp_add_pwd[0]); 
  
  vtcp_log("ǰ̨������   �����= packid [%.8s]!!!\n",fd123.packid);
  
  
  vtcp_log("ǰ̨������   ԭ�����= o_packid [%.8s]!!!\n",fd123.o_packid);
  
  vtcp_log("ǰ̨������   ������= pack_date [%.8s]!!!\n",fd123.pack_date);
  
  vtcp_log("ǰ̨������   ԭ������= o_pack_date [%.8s]!!!\n",fd123.o_pack_date);
  
  vtcp_log("ǰ̨������   ҵ�����ͱ�� = txnum [%.5s]!!!\n",fd123.txnum);
  
  vtcp_log("ǰ̨������   ԭҵ�����ͱ��= o_txnum [%.5s]!!!\n",fd123.o_txnum);
  
  vtcp_log("ǰ̨������   С��ҵ�������,2λ��Ż���12λ���� = lv_yw_ind [%.12s]!!!\n",fd123.lv_yw_ind);
  
  vtcp_log("ǰ̨������   Ӧ������ = resp_date [%.8s]!!!\n",fd123.resp_date);
  
  vtcp_log("ǰ̨������   ҵ���ִ״̬ = rcpstat [%.2s]!!!\n",fd123.rcpstat);
  
  vtcp_log("ǰ̨������   ccpc���״������ = resp2 [%.8s]!!!\n",fd123.resp2);
  
  vtcp_log("ǰ̨������   �˻�ԭ��= retcode [%.2s]!!!\n",fd123.retcode);
  
  vtcp_log("ǰ̨������   ��ϸ��� = ywdtlid [%.8s]!!!\n",fd123.ywdtlid);
  
  vtcp_log("ǰ̨������   ʵʱҵ�񸶿����ʻ����� = actype [%c]!!!\n",fd123.actype);
  
  vtcp_log("ǰ̨������   �������� 0������  1������ = reqtype [%c]!!!\n",fd123.reqtype[0]);
  
  vtcp_log("ǰ̨������   �Է������� = df_chrg_amt [%.16s]!!!\n",fd123.df_chrg_amt);
  
  vtcp_log("ǰ̨������   �෽Э���= prot_no [%.60s]!!!\n",fd123.prot_no);
  
  vtcp_log("ǰ̨������   ��Ʊ���� = po_no [%.20s]!!!\n",fd123.po_no);
  
  vtcp_log("ǰ̨������   ��Ʊ����0��ת 1 ����ת 2�ֽ� = po_type [%c]!!!\n",fd123.po_type[0]);
  
  vtcp_log("ǰ̨������   ��С���־ 0��1С�� = otype  [%c]!!!\n",fd123.otype[0]);
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
   
   
   
  vtcp_log("[%s][%d]�������ȡ�������е�FD123�����������!\n",__FILE__,__LINE__);
  
  
  get_fd_data("0030",hv_notpay->F_tx_br_no);
  vtcp_log("ǰ̨�����Ľ��׻��� F_tx_br_no=[%.5s]",hv_notpay->F_tx_br_no); 
  
  
  
  
  memcpy(hv_notpay->F_wt_date,pcLvGetDay(),sizeof(hv_notpay->F_wt_date));
  /********ί�����ڱ�����С������,���ǽ��������Ծ�Ϊ�ۺ�ϵͳ����*******/
  vtcp_log("ǰ̨������ ί������ wt_date=[%.8s]",hv_notpay->F_wt_date); 
  
  
  get_fd_data("0440",hv_notpay->F_tx_date);
  vtcp_log("ǰ̨������ �������� F_tx_date=[%.8s]",hv_notpay->F_tx_date); 
  
  
  get_fd_data("0210",cCur_no);
  vCur_dhcc_mbfe(cCur_no,hv_notpay->F_o_cur_no);
  vtcp_log("ǰ̨������ ���� F_o_cur_no=[%.3s]",hv_notpay->F_o_cur_no); 
  
  
  get_fd_data("0390",hv_notpay->F_o_txamt);
  vtcp_log("ǰ̨������  ���׽��txamt=[%.16s]",hv_notpay->F_o_txamt); 
  
  
  
  
  get_fd_data("0460",hv_notpay->F_o_fzf_date);/* ԭ��֧����������,�鸴���õ��Ĳ�ѯ���� */    
  vtcp_log("ǰ̨������ ԭ��֧���������� F_o_fzf_date=[%.8s]",hv_notpay->F_o_fzf_date); 
  
  
  hv_notpay->F_otype[0] = fd123.otype[0];
  vtcp_log("ǰ̨������ ѡ���С�� 0 ��� 1 С�� hv_notpay->F_otype[0]=[%c]",hv_notpay->F_otype[0]); 
  
  
  get_fd_data("0450",hv_notpay->F_o_wt_date);
  vtcp_log("ǰ̨������  ԭί������o_wt_date=[%.8s]",hv_notpay->F_o_wt_date);
  
  
  if(memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
  {
    iGetHvRefno(HV_QS_BR_NO,hv_notpay->F_refno);
  }
  vtcp_log("��ǰ̨��ȡ�ı��Ĳο��� hv_notpay->F_refno=[%s]",hv_notpay->F_refno);
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  
  
  /****************Ҫ��ʲô�ر��� ������������!**********************/
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  memcpy(cOr_br_no,hv_notpay->F_or_br_no,sizeof(cOr_br_no)-1);
  zip_space(cOr_br_no);
  vtcp_log("��ǰ̨��ȡ�ı����к�(���ɵ�)F_or_br_no=[%.12s]\n",hv_notpay->F_or_br_no);
  if(cOr_br_no[0]==0x00)
  {
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    GetOr_br_noByBr_no(hv_notpay->F_tx_br_no,hv_notpay->F_or_br_no);
    vtcp_log("��ǰ̨��ȡ�ı����к�(���ɵ�)F_or_br_no=[%.12s]\n",hv_notpay->F_or_br_no);
  }
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  
  
  if(memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
  {
    get_fd_data("0070",hv_notpay->F_tel);
    vtcp_log("ǰ̨������ ����Ա��tel=[%s]",hv_notpay->F_tel); 
  }
  else if(memcmp(hv_notpay->F_opcd,"40",sizeof(hv_notpay->F_opcd))==0)
  {
    get_fd_data("0070",hv_notpay->F_send_tel);
    vtcp_log("ǰ̨������ ����Ա�� tel=[%s]",hv_notpay->F_send_tel); 
  }
  
  
  if(memcmp(cCmtno,"301",3)==0 )/**��Ҫʱ�����**/
  {
    /*ԭ֧�����׷�����*/
    memcpy(hv_notpay->F_o_or_br_no,fd123.ac_br_no,sizeof(hv_notpay->F_o_or_br_no));
    /*ԭ֧�����׽�����*/
    memcpy(hv_notpay->F_o_ac_br_no,fd123.or_br_no,sizeof(hv_notpay->F_o_ac_br_no));
  }
  vtcp_log("[%s][%d]ִ�к��� iLvTistoNotpay �ɹ�!\n",__FILE__,__LINE__);
  return 0;
}


int iLvGetFzfOrderno()
{
  char cOrderno[9];
  int  iOrderno;
  int  ret = 0;
  memset(cOrderno,0x00,sizeof(cOrderno));
  
  ret =iGetHvFzfOrderno(cOrderno);
  if(ret)
  {
    vtcp_log("[%s][%d]��ȡ������Ŵ���!\n",__FILE__,__LINE__);
    return (-1);
  }
  
  
  iOrderno = apatoi(cOrderno,sizeof(cOrderno)-1);
  
  vtcp_log("[%s][%d]iOrderno=[%lf]\n",__FILE__,__LINE__,iOrderno);
  
  return iOrderno;
}    
                 
