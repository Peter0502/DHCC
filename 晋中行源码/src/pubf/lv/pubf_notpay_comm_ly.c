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
* 函 数 名: iLvUpdFzfRecFromPayin                  
* 功能描述： 根据 非支付结构 T 部分 更新 hv_fzf    
* 作    者:  ChenMing                             
* 完成日期： 2006-8-9                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int iLvUpdFzfRecFromPayin(HV_NOTPAY_AREA *phv_notpayin)
{            

  int ret=0;
  struct hv_fzf_c hv_fzf;
  memset(&hv_fzf, 0x00, sizeof(hv_fzf));

  vtcp_log("iHvUpdFzfRecFromPayin() 开始 \n");
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
      sprintf(acErrMsg,"查询非支付交易表错误!![%d]",ret);
      WRITEMSG
        return ret;
    }
    else if (ret == 100)
    {
      sprintf(acErrMsg,"非支付交易表中不存在该记录!!");
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
* 函 数 名: iLvNotPayin_Copy                    
* 功能描述: 将HV_NOTPAY_AREA中相应字段值赋给hv_fzf_c中相应字段 
* 作    者:  ChenMing                            
* 完成日期： 2006-8-9                            
* 修改记录： liuyue 加入了原包序号以及原包ID字段                                    
*   日   期:                                      
*   修 改 人:                                    
*   修改内容:                                    
*************************************************************************/  
int iLvNotPayin_Copy(HV_NOTPAY_AREA *phv_notpayin, struct hv_fzf_c *hv_fzf, LV_NOTPAY_ADD *lv_add)
{
  memcpy(hv_fzf->cmtno,phv_notpayin->T_cmtno,sizeof(hv_fzf->cmtno)-1);
  
  memcpy(hv_fzf->or_br_no,phv_notpayin->T_or_br_no,sizeof(hv_fzf->or_br_no)-1);
  if ( strlen(phv_notpayin->T_wt_date) > 0 )  
    hv_fzf->wt_date = apatoi(phv_notpayin->T_wt_date,8);
  memcpy(hv_fzf->orderno,phv_notpayin->T_orderno,sizeof(hv_fzf->orderno)-1);
  
    vtcp_log(" phv_notpayin->T_otype[%s] ",phv_notpayin->T_otype);    
    hv_fzf->otype[0] = '2';/****小额业务***/
  
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
  hv_fzf->o_tx_date = apatoi(phv_notpayin->T_o_wt_date,8);   /* 请在处理时保证任何时候o_wt_date和o_tx_date一致 */
  hv_fzf->o_wt_date = apatoi(phv_notpayin->T_o_wt_date,8);   /* 请在处理时保证任何时候o_wt_date和o_tx_date一致 */

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
  hv_fzf->lw_ind[0] = phv_notpayin->T_lw_ind[0];/* 来往标识 1-往 2-来  */
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__); 


  /****另外对于小额还需要插入下面几个字段*******/
  vtcp_log("[%s][%d]funk!\n",__FILE__,__LINE__);
  vtcp_log("[%s][%d] hv_fzf->reqtype[0]=[%c]  lv_add->reqtype[0]=[%c]",__FILE__,__LINE__,hv_fzf->reqtype[0],lv_add->reqtype[0]);  
  hv_fzf->reqtype[0] = lv_add->reqtype[0];/***单笔还是整包的情况****/
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  
  memcpy(hv_fzf->o_packid,lv_add->opackid,sizeof(hv_fzf->o_packid)-1);/**原包日期***/
  vtcp_log("[%s][%d]hv_fzf->o_packid===[%s]\n",__FILE__,__LINE__,hv_fzf->o_packid);
  vtcp_log("[%s][%d]lv_add->opack_date==[%s]\n",__FILE__,__LINE__,lv_add->opack_date);
  hv_fzf->o_pack_date = apatoi(lv_add->opack_date,sizeof(lv_add->opack_date));
  vtcp_log("[%s][%d]hv_fzf.o_pack_date==[%ld]\n",__FILE__,__LINE__,hv_fzf->o_pack_date);
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  memcpy(hv_fzf->otxnum,lv_add->otxnum,sizeof(hv_fzf->otxnum)-1);/***原业务编号***/
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);  
    /* */
    vtcp_log("[%s][%d]执行函数iLvNotPayin_Copy 成功结束!\n",__FILE__,__LINE__);
    return 0;  
}
/***********************************************************************     
* 函 数 名: iLvFzfToFzfhtr                    
* 功能描述: 将hv_fzf中相应字段值赋给hv_fzfhtr
* 作    者:  liuyue                            
* 完成日期： 2006-9-12                                                             
*************************************************************************/ 
int iLvFzfToFzfhtr(struct hv_fzf_c *hv_fzf,struct hv_fzfhtr_c *hv_fzfhtr)
{
  vtcp_log("[%s][%d]开始进行从hv_fzf到hv_fzfhtr的复制!\n",__FILE__,__LINE__);
  
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
  vtcp_log("[%][%d]复制完毕!\n",__FILE__,__LINE__);
  
  return 0;
}



/************************************************************************* 
* 函 数 名: iLvFzFCommonCheck                  
* 功能描述：对非支付交易进行一些公共的检测 (采用了大额非支付结构体)
* 作    者:  liuyue                             
* 完成日期： 2006-9-13                            
* 修改记录：                                      
* 日   期:                                      
* 修 改 人:                                     
* 修改内容:                                      
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
  
  
  /*****检测CMT编号是否合法******/
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno非法!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P065");
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]非法的CMTNO号!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P065");
    return (-1);
  }
  
  
  /******检查是否存在非法字符*****/
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
           vtcp_log("[%s][%d]输入的附言中存在非法内容!请您重新输入!\n",__FILE__,__LINE__);
           vtcp_log("[%s][%d]content=[%s]\n",__FILE__,__LINE__,hv_notpayin->F_content);
           strcpy(g_pub_tx.reply,"P070");
           return (-1);
         }
         break;
  }
  
  
  /*****检查营业状态*******/
  ret = iLv_checkstat(cCmtno,"");
  if(ret)
  {
    vtcp_log("[%s][%d]营业状态检查错误!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P066");
    return (-1);
  }
  
  
  /********检测行号是否合法****/
  ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no,cOr_br_no);
  if(ret)
  {
    vtcp_log("[%s][%d]通过机构号查找发起行错误!\n",__FILE__,__LINE__);
    return (-1);
  }
  ret = iGet_full_hv_no(cOr_br_no, cSendco,cPay_qsno);
  if(ret)
  {
    vtcp_log("[%s][%d]通过行号查找发报中心,发起清算行错误\n",__FILE__,__LINE__);
    return (-1);
  }
  zip_space(cAc_br_no);
  if(strlen(cAc_br_no)==12)/***区分自由格式的时候接受行位4位收报中心***/
  {
    ret = iGet_full_hv_no(cAc_br_no,cRececo,cCash_qsno);
    if(ret)
    {
      vtcp_log("[%s][%d]通过接受行找收报中心,接受清算行错误!\n",__FILE__,__LINE__);
      return (-1);
    }
    /** NEWYX by LiuHuafeng 2007-4-4 6:09 都可以
    if(!memcmp(cOr_br_no,cAc_br_no,sizeof(cOr_br_no)-1))
    {
      vtcp_log("[%s][%d]发起行和接受行,不能是同一行!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"P067");
      return (-1);
    }
    **************************************************/
    /*** 票据影像可以的dongxy 20070328
    if(!memcmp(cPay_qsno,cCash_qsno,sizeof(cPay_qsno)-1))
    {
      vtcp_log("[%s][%d]发起清算行和接受清算行不能是同一行!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"P068");
      return (-1);
    }
    ***/
  }
  
  /********判断一下日期是否合法
  memcpy(cWt_date,hv_notpayin->F_o_wt_date,sizeof(hv_notpayin->F_o_wt_date));
  if(strlen(cWt_date)!=0)
  {
    if(memcmp(cWt_date,hv_notpayin->F_wt_date,sizeof(hv_notpayin->F_wt_date))>0)
    {
      vtcp_log("原委托日期不合法owtdate=[%.8s]\n",cWt_date);
      strcpy(g_pub_tx.reply,"P069");
      return (-1);
    }
  }
  *********/
  
  return 0;
  
}


/************************************************************************* 
* 函 数 名: iLvFzFCommonCheck2                  
* 功能描述：对非支付交易进行一些公共的检测 (采用了小额支付结构体)
* 作    者:  liuyue                             
* 完成日期： 2006-9-18                            
* 修改记录：                                      
* 日   期:                                      
* 修 改 人:                                     
* 修改内容:                                      
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
    vtcp_log("[%s][%d]cmtno非法!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cPkgno,"012",3)&&memcmp(cPkgno,"013",3))
  {
    vtcp_log("[%s][%d]非法的pkgno号!\n",__FILE__,__LINE__);
    return (-1);
  }
  
  ret = iLv_checkstat(cPkgno,"");
  if(ret)
  {
    vtcp_log("[%s][%d]营业状态检查错误!\n",__FILE__,__LINE__);
    return (-1);
  }
  
  
  
  ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no,cOr_br_no);
  if(ret)
  {
    vtcp_log("[%s][%d]通过机构号查找发起行错误!\n",__FILE__,__LINE__);
    return (-1);
  }
  vtcp_log("[%s][%d]cOr_br_no==[%.12s]\n",__FILE__,__LINE__,cOr_br_no);
  ret = iGet_full_hv_no(cOr_br_no, cSendco,cPay_qsno);
  if(ret)
  {
    vtcp_log("[%s][%d]通过行号查找发报中心,发起清算行错误\n",__FILE__,__LINE__);
    return (-1);
  }
  zip_space(cAc_br_no);
  if(strlen(cAc_br_no)==12)/***区分自由格式的时候接受行位4位收报中心***/
  {
    ret = iGet_full_hv_no(cAc_br_no,cRececo,cCash_qsno);
    if(ret)
    {
      vtcp_log("[%s][%d]通过接受行找收报中心,接受清算行错误!\n",__FILE__,__LINE__);
      return (-1);
    }
    if(!memcmp(cOr_br_no,cAc_br_no,sizeof(cOr_br_no)-1))
    {
      vtcp_log("[%s][%d]发起行和接受行,不能是同一行!\n",__FILE__,__LINE__);
      return (-1);
    }
    /**NEWYX rem by LiuHuafeng 20070403 23:59
    if(!memcmp(cPay_qsno,cCash_qsno,sizeof(cPay_qsno)-1))
    {
      vtcp_log("[%s][%d]发起清算行和接受清算行不能是同一行!\n",__FILE__,__LINE__);
      return (-1);
    }
    **NEWYX rem by LiuHuafeng 20070404 */
  }
  
  /********判断一下日期是否合法*********/
  memcpy(cWt_date,lv_payin->F_owtday,sizeof(lv_payin->F_owtday));
  if(strlen(cWt_date)!=0)
  {
    if(memcmp(cWt_date,lv_payin->F_wtday,sizeof(lv_payin->F_wtday))>0)
    {
      vtcp_log("原委托日期不合法owtdate=[%.8s]\n",cWt_date);
      return (-1);
    }
  }
  
  return 0;
  
}
/***********************************************************************     
* 函 数 名: iLvJudge_Change_notpay_stat(strcut hv_fzf_c *hv_fzf)                    
* 功能描述: 判断非支付交易记录的状态，并根据交易更新状态,所有的状态更改全部放到这里来
* 作    者:  liuyue                            
* 完成日期： 2006-9-12                                                             
*************************************************************************/ 
int iLvJudge_Change_notpay_stat(struct hv_fzf_c *hv_fzf,char *cmtno) 
{
  vtcp_log("[%s][%d]判断---改变非支付交易记录的状态!\n",__FILE__,__LINE__);
  char cCmtno[4];
  int  iCmtno   ;
  memset(cCmtno,0x00,sizeof(cCmtno));
  
  memcpy(cCmtno,cmtno,sizeof(cCmtno)-1);
  
  vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,cCmtno);
  zip_space(cCmtno);
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno非法!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]非法的CMTNO号!\n",__FILE__,__LINE__);
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
    case 327:       /**申请类的交易*****/
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
          vtcp_log("需要状态[%c]实际状态[%c]\n",STAT_LZRECV1,hv_fzf->hv_fzf_sts[0]);
          return (-1);
        }
        vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
        /*状态先不打算变了****/
        /*hv_fzf->hv_fzf_sts[0] = STAT_LZBACK;*/
        break;
    default:
        vtcp_log("[%s][%d]没有任何动作!\n",__FILE__,__LINE__);
  }
  vtcp_log("[%s][%d] stat=[%c]\n",__FILE__,__LINE__,hv_fzf->hv_fzf_sts[0]);
  
  
  return 0;
}

/***********************************************************************     
* 函 数 名: iLvJudge_Change_payin_stat(strcut lv_pkgreg_c *lv_pkgreg，char *cmtno)                    
* 功能描述: 判断小额支付交易记录的状态，并根据交易更新状态,所有的状态更改全部放到这里来
* 作    者:  liuyue                            
* 完成日期： 2006-9-12                                                             
*************************************************************************/ 
int iLvJudge_Change_payin_stat(struct lv_pkgreg_c *lv_pkgreg,char *cmtno)  
{
  vtcp_log("[%s][%d]判断---改变支付交易记录的状态!\n",__FILE__,__LINE__);
  char cCmtno[4];
  int  iCmtno   ;
  memset(cCmtno,0x00,sizeof(cCmtno));
  
  memcpy(cCmtno,cmtno,3);
  
  vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,cCmtno);
  zip_space(cCmtno);
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno非法!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]非法的CMTNO号!\n",__FILE__,__LINE__);
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
          vtcp_log("[%s][%d]退回申请,原支付交易记录错误!\n",__FILE__,__LINE__);
          vtcp_log("table=[%c] [%c][%c]\n",lv_pkgreg->lv_sts[0],STAT_WZSUCCESS,STAT_WZSEND);
          return (-1);
        }
        break;
    case 321:
        if(lv_pkgreg->lv_sts[0]!=STAT_WZINQUEUE&&lv_pkgreg->lv_sts[0]!=STAT_WZSEND)
        {
          vtcp_log("[%s][%d]退回申请,原支付交易记录错误!\n",__FILE__,__LINE__);
          vtcp_log("table=[%c] [%c][%c]\n",lv_pkgreg->lv_sts[0],STAT_WZINQUEUE,STAT_WZSEND);
          return (-1);
        }
        break;
    case 323:
    case 327:       /**申请类的交易*****/
        if(lv_pkgreg->lv_sts[0]!=STAT_WZSEND)
        {
          vtcp_log("[%s][%d]退回申请,原支付交易记录错误!\n",__FILE__,__LINE__);
          vtcp_log("table=[%c] [%c]\n",lv_pkgreg->lv_sts[0],STAT_WZSEND);
          return (-1);
        }
        break;
    default:
        vtcp_log("[%s][%d]没有任何动作!\n",__FILE__,__LINE__);
  }
  
  return 0;
}

/*************************************************************************/
/* 函数名称：void lv_checkstat()                                         */
/*                                                                       */
/* 功能描述：检查当前营业状态                                            */
/*                                                                       */
/* 输入：无                                                              */
/*                                                                       */
/* 返回值： 无                                                           */
/* 说明：  根据TITA检查当前状态是否允许做此（大额）业务，如果不允许，设置*/
/*         it_txcom.txrsut和错误描述，但不调用gs99或gs100                */
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
  vtcp_log("%s,%d在这里设置1===1,cmtno=[%s]\n",__FILE__,__LINE__,cmtno);
  
  ret=Lv_sysctl_Sel(cReply,&lv_sysctl,"1=1");
  if(ret!=0){
    vtcp_log("%s,%d,取得小额营业状态错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
    strncpy(acErrMsg,"取得小额营业状态错误",40);
    set_zd_data("0130",acErrMsg);
    return(-1);
  }
  memset(&com_sys_parm, 0 , sizeof(com_sys_parm));
  ret=Com_sys_parm_Sel(cReply,&com_sys_parm,"1=1");
  if(ret!=0){
    vtcp_log("%s,%d,取得公共系统参数表错误,sqlcode=%d\n",__FILE__,__LINE__,ret);
    strncpy(acErrMsg,"取得公共系统参数表错误",40);
    return(-1);
  }

  if(com_sys_parm.sys_sts!=0)
  {
   if((com_sys_parm.sys_sts==1 || com_sys_parm.sys_sts==2) && memcmp(g_pub_tx.tx_code,"6388",4)==0)
    {
    vtcp_log("%s,%d,网银小额业务允许在核心状态为1和2的状态下进行,tx_code=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
    }
    else
    {
    vtcp_log("%s,%d,当前状态不可发起小额业务,sqlcode=%d\n",__FILE__,__LINE__,ret);
    strncpy(acErrMsg,"当前状态不可发起小额业务",40);
    set_zd_data("0130",acErrMsg);
    return(-1);
    }
  }
  switch(lv_sysctl.stat[0]){
  case LV_STAT_PREPARE: 
    /*营业准备状态,只能接收，不能发起业务*/
    if(memcmp(cmtno,"500",3))
    {
      vtcp_log("%s,%d,营业准备状态不能发起交易\n",__FILE__,__LINE__);
      strncpy(acErrMsg,"营业准备状态不能发起交易",40);
      return 1;
    }
    else
    {
      vtcp_log("%s,%d 500报文不查状态 ",__FILE__,__LINE__);
      return 0;
    }
  case LV_STAT_WORK:
    /*日间营业状态*/
    vtcp_log("%s,%d,日间营业状态能发起所有交易\n",__FILE__,__LINE__);
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
    vtcp_log("%s,%d 当前系统状态错误",__FILE__,__LINE__);
    return iRet;
  }
}
/*************************************************************************/
/* 函数名称：int lv_check_notpay_record(HV_NOTPAY_AREA *hv_notpayin.....)*/
/*                                                                       */
/* 功能描述：为应答类的交易检测原申请类交易是否存在                      */
/*           并更新原非支付交易的记录                                    */
/*                                                                       */
/* 返回值： 0-成功 1-失败                                                */
/*************************************************************************/
int  lv_check_notpay_record(HV_NOTPAY_AREA *hv_notpay,struct hv_fzf_c *hv_fzf,char *cmtno) 
{
  vtcp_log("[%s][%d]开始检测原非支付交易是否存在!\n",__FILE__,__LINE__);
  
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
    vtcp_log("[%s][%d]cmtno非法!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]非法的CMTNO号!\n",__FILE__,__LINE__);
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
        vtcp_log("[%s][%d]查找非支付交易记录错误!ret=[%d]\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P015");
        return (-1);
      }
      
      ret = Hv_fzf_Fet_Upd(hv_fzf,g_pub_tx.reply);
      if(ret&&ret!=100)
      {
        vtcp_log("[%s][%d]数据库操作错误\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P015");
        return (-1);
      }
      else if(ret==100)
      {
        vtcp_log("[%s][%d]原非支付交易记录不存在!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P017");
        return (-1);
      }
      
      /**判断和设置状态*****/
      vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
      
      vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
      
      ret = iLvJudge_Change_notpay_stat(hv_fzf,cCmtno);
      if(ret)
      {
        vtcp_log("[%s][%d]判断和设置原非支付记录状态错误!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P052");
        return (-1);
      }
      vtcp_log("[%s][%d]生成的新的小额交易序号＝【%d】\n",__FILE__,__LINE__,iOrderno);
      /***提取交易序号***/
      iOrderno=iLvGetFzfOrderno();
      if(iOrderno<=0)
      {
        sprintf( acErrMsg,"得到支付交易序号错误![%s][%d]",__FILE__,__LINE__);
        WRITEMSG
        return (-1);
      }
      vtcp_log("[%s][%d]生成的新的小额交易序号＝【%d】\n",__FILE__,__LINE__,iOrderno);
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
        vtcp_log("[%s][%d]更新原非支付交易记录失败!ret=[%d]\n",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"P055");
        return (-1);
      }
    
    
      vtcp_log("[%s][%d]hv_fzf.res_orderno=[%s]\n",__FILE__,__LINE__,hv_fzf->res_orderno);
      vtcp_log("[%s][%d]hv_fzf.res_brno=[%s]\n",__FILE__,__LINE__,hv_fzf->res_br_no);
      vtcp_log("[%s][%d]hv_fzf.res_date=[%ld]\n",__FILE__,__LINE__,hv_fzf->res_date);

      
      vtcp_log("hv_fzf===[%s]\n",(char *)hv_fzf);
      Hv_fzf_Clo_Upd();
      vtcp_log("[%s][%d]对原非支付交易记录的处理结束!\n",__FILE__,__LINE__);
      break;
    
  }
  
  
  
  return (0);
}


/*************************************************************************/
/* 函数名称：int  lv_check_payin_record(HV_NOTPAY_AREA *hv_notpayin,     */
/*             LV_NOTPAY_ADD *lv_addinfo,char *cmtno,char cReq_type)     */
/*                                                                       */
/* 功能描述：检测与非支付交易对应的支付交易是否存在                      */
/*           检测支付交易状态  需要就更新支付交易状态                    */
/*           如果记录存在 提取 与包相关的信息到lv_addinfo中              */
/*           如果前台不输入接受行,从支付交易中提取接受行                 */
/*                                                                       */
/* 返回值： 0-成功 1-失败                                                */
/*************************************************************************/

int  lv_check_payin_record(HV_NOTPAY_AREA *hv_notpayin,LV_NOTPAY_ADD *lv_addinfo,char *cmtno,char cReq_type)
{
  vtcp_log("[%s][%d]开始对小额的支付交易进行检测!\n",__FILE__,__LINE__);
  
  struct lv_pkgreg_c lv_pkgreg;
  char cCmtno[4];
  int  iCmtno = 0;
  int  ret = 0;
  int  iRecordNum = 0;
  char cOr_br_no[13];
  char cAc_br_no[13];
  char cPack_date[9];
  
  char cF_ac_br_no[13];/***提取notpayin中的F_ac_br_no******/
  
  
  int  iQsno = 0;/***当前交易行是否是清算中心，1 是 0 不是，处理整包时候用到****/
  int  iAcbrno = 1;/**判断整包中是否是给同一接受行，是将对hv_notpay->F_ac_br_no赋值***/
  /*如果是发给多个行的 将对F_ac_br_no赋值为清算中心 1 相邻两笔记录同接受行 0 不同接受行**/
  int  iSame   = 1;/*0-存在多个接受行 1 发送给一个支行的***/
  
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
  vtcp_log("[%s][%d]当前行是0 不是清算中心 1 是清算中心  【%d】\n",__FILE__,__LINE__,iQsno);
      
  memcpy(cCmtno,cmtno,3);
  iCmtno = atoi(cCmtno);
  
  vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,cCmtno);
  zip_space(cCmtno);
  if(strlen(cCmtno)!=3)
  {
    vtcp_log("[%s][%d]cmtno非法!\n",__FILE__,__LINE__);
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"302",3)&&memcmp(cCmtno,"303",3)
    &&memcmp(cCmtno,"319",3)&&memcmp(cCmtno,"320",3)&&memcmp(cCmtno,"321",3)&&memcmp(cCmtno,"322",3)
    &&memcmp(cCmtno,"323",3)&&memcmp(cCmtno,"324",3)&&memcmp(cCmtno,"325",3)&&memcmp(cCmtno,"327",3)
    &&memcmp(cCmtno,"328",3))
  {
    vtcp_log("[%s][%d]非法的CMTNO号!\n",__FILE__,__LINE__);
    return (-1);
  }
  vtcp_log("[%s][%d]ccmtno=[%.3s]\n",__FILE__,__LINE__,cCmtno);
  vtcp_log("[%s][%d]reqtype==[%c]\n",__FILE__,__LINE__,cReq_type);
  
  
  vtcp_log("[%s][%d]初始化的：hv_notpayin.F_ac_br_no===[%.12s]\n",__FILE__,__LINE__,cF_ac_br_no);
  
  if(cReq_type=='0')
  {
    vtcp_log("[%s][%d]前台输入的是单笔的处理!\n",__FILE__,__LINE__);
    memcpy(lv_pkgreg.orderno,hv_notpayin->F_o_payin_orderno,sizeof(lv_pkgreg.orderno)-1);
    memcpy(lv_pkgreg.or_br_no,hv_notpayin->F_or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
    lv_pkgreg.wt_date = apatoi(hv_notpayin->F_o_wt_date,sizeof(hv_notpayin->F_o_wt_date));
    
    ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"orderno='%.8s' and or_br_no='%.12s' and wt_date=%d ",
        lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date);
    if(ret)
    {
      vtcp_log("[%s][%d]数据库操作错误 !\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"D103");
      return (-1);
    }
    
    ret = Lv_pkgreg_Fet_Upd(&lv_pkgreg,g_pub_tx.reply);
    if(ret&&ret!=100)
    {
      vtcp_log("[%s][%d]定义游标错误!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"D101");
      return (-1);
    }
    else if(ret==100)
    {
      vtcp_log("[%][%d]记录不存在!\n",__FILE__,__LINE__);
      if(iCmtno==301)
      {
        vtcp_log("[%s][%d]这是查询交易,可以允许记录不存在情况!\n",__FILE__,__LINE__);
        Lv_pkgreg_Clo_Upd();
        return 0;
      }
      else
      {
        vtcp_log("[%s][%d]原支付交易记录不存在!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P017");
        return (-1);
      }
    }
    vtcp_log("[%s][%d]到这里说明原支付交易记录是存在的!\n",__FILE__,__LINE__);
    ret = iLvJudge_Change_payin_stat(&lv_pkgreg,cCmtno);
    if(ret)
    {
    vtcp_log("[%s][%d]处理小额支付交易记录错误!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P057");
    return (-1);
    }
    
    ret = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
    if(ret)
    {
      vtcp_log("[%s][%d]更新小额支付交易记录错误!\n",__FILE__,__LINE__);
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
    
    
    
    /**********对于比如319之类的交易,其接受行要从支付交易中找的******/
    /*add by liuyue 20060925*******/
    if(cF_ac_br_no[0]=='\0')
    {  
      vtcp_log("[%s][%d]hv_notpayin->F_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpayin->T_cash_qs_no);
      /* 接收行行号 */
      memcpy(hv_notpayin->F_ac_br_no  ,lv_pkgreg.ac_br_no   ,sizeof(hv_notpayin->F_ac_br_no));
      vtcp_log("[%s][%d]!!!!从支付交易中提取了接受行!\n",__FILE__,__LINE__);
      
    }
    /************add end******************/
    
    vtcp_log("[%s][%d]对于单笔的情况已经处理完毕!\n",__FILE__,__LINE__);  
  
  }
  else if(cReq_type=='1')
  {
    vtcp_log("[%s][%d]开始处理的是整包的交易!\n",__FILE__,__LINE__);
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
      vtcp_log("[%s][%d]数据库操作错误!\n",__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"D103");
      return (-1);
    }
    
    while(1)
    {
      ret = Lv_pkgreg_Fet_Upd(&lv_pkgreg,g_pub_tx.reply);
      vtcp_log("[%s][%d]这是第【%d】笔交易\n",__FILE__,__LINE__,iRecordNum);
      if(ret&&ret!=100)
      {
        vtcp_log("[%s][%d]数据库操作错误!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"D101");
        return (-1);
      }
      
      else if(iRecordNum==0&&ret ==100)
      {
        vtcp_log("[%s][%d]原支付交易记录不存在!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P017");
        return (-1);
      }
      else if(iRecordNum&&ret==100)
      {
        vtcp_log("[%s][%d]游标已经取到最后了!\n",__FILE__,__LINE__);
        break;
      }
      
      
      /****将第一笔记录的接受行放到cAc_br_no中****/
      if(iRecordNum==0)
      {
        memcpy(cAc_br_no,lv_pkgreg.ac_br_no,sizeof(lv_pkgreg.ac_br_no)-1);
      }
      vtcp_log("[%s][%d]第【%d】笔记录的ac_br_no==[%s]\n",__FILE__,__LINE__,iRecordNum,lv_pkgreg.ac_br_no);
      
      if(iRecordNum&&!memcmp(cAc_br_no,lv_pkgreg.ac_br_no,sizeof(cAc_br_no)-1))
      {
        iAcbrno = 1;/***这两笔记录的接受行是一致的***/
        vtcp_log("[%s][%d]整包中这两笔记录的接受行一致!\n",__FILE__,__LINE__);
      }
      else if(iRecordNum&&memcmp(cAc_br_no,lv_pkgreg.ac_br_no,sizeof(cAc_br_no)-1))
      {
        iAcbrno = 0;/***这两笔记录的接受行是不一致的***/
        vtcp_log("[%s][%d]整包中这两个记录的接受行不一致!\n",__FILE__,__LINE__);
      }
      /***接着将当前记录的ac_br_no赋值给cAcbrno*****/
      memcpy(cAc_br_no,lv_pkgreg.ac_br_no,sizeof(lv_pkgreg.ac_br_no)-1);
      
      iSame  =  iSame*iAcbrno;
      
      /***初始化的时候cOr_br_no中已经存放了本行的的12位行号******/
      /****处理对于整包业务，如果其中包含有他行业务,有清算中心来完成****/
      vtcp_log("[%s][%d]cOr_br_no=[%s] lv_pkgreg.or_br_no=[%s] orderno===[%.8s]\n",__FILE__,__LINE__,cOr_br_no,lv_pkgreg.or_br_no,lv_pkgreg.orderno);
      if(!iQsno&&memcmp(cOr_br_no,lv_pkgreg.or_br_no,sizeof(cOr_br_no)-1))
      {
        vtcp_log("[%s][%d]整包中涉及到多个行,只能由清算中心做此交易!\n",__FILE__,__LINE__);
        vtcp_log("[%s][%d]两个行:[%s][%s]\n",__FILE__,__LINE__,cOr_br_no,lv_pkgreg.or_br_no);
        strcpy(g_pub_tx.reply,"P059");
        return (-1);
      }
      
      /***取当前记录的or_br_no********/
      memset(cOr_br_no,0x00,sizeof(cOr_br_no));
      memcpy(cOr_br_no,lv_pkgreg.or_br_no,sizeof(cOr_br_no)-1);
      
      
      iRecordNum++;
      
      vtcp_log("[%s][%d]这是整包里面的第【%d】笔业务了!\n",__FILE__,__LINE__,iRecordNum);
      
      vtcp_log("[%s][%d]到这里说明原支付交易记录是存在的!\n",__FILE__,__LINE__);
      ret = iLvJudge_Change_payin_stat(&lv_pkgreg,cCmtno);
      if(ret)
      {
        vtcp_log("[%s][%d]处理小额支付交易记录错误!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P057");
        return (-1);
      }
    
      ret = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
      if(ret)
      {
        vtcp_log("[%s][%d]更新小额支付交易记录错误!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P058");
        return (-1);
      }
      
    }
    
    
    if(iCmtno==323&&iRecordNum>1)
    {
      vtcp_log("[%s][%d]冲正交易虽然是整包的,但是整包中只有一笔!\n");
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
    
    /***处理退回申请等交易,接受行需要从支付记录中找的情况****/
    
    
    vtcp_log("[%s][%d]iSame==[%d] 1-整包发送给同一行  0-整包发送给不同的行\n",__FILE__,__LINE__,iSame);
    if(iSame==1 && cF_ac_br_no[0]==0x00)/****如果整包是发给一个支行的***/
    {
      memcpy(hv_notpayin->F_ac_br_no,lv_pkgreg.ac_br_no,sizeof(lv_pkgreg.ac_br_no)-1);
      vtcp_log("[%s][%d]提取接受行到F_ac_br_no==[%s]!\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
    }
    else if(iSame==0 && cF_ac_br_no[0]==0x00)/**这是发给好几个支行的,添上清算中心**/
    {
      memcpy(hv_notpayin->F_ac_br_no,lv_pkgreg.cash_qs_no,sizeof(lv_pkgreg.cash_qs_no)-1);
      vtcp_log("[%s][%d]提取接受清算中心到接受行![%s]\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
    }
      
    vtcp_log("[%s][%d]hv_notpayin->F_ac_br_no==[%s]\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
    
    vtcp_log("[%s][%d]对于整笔的情况已经处理完毕!\n",__FILE__,__LINE__);
  }
  else
  {
    vtcp_log("[%s][%d]错误的单笔,整包类型!\n",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"P060");
    return (-1);
  }
  
  vtcp_log("[%s][%d]重新获取后:hv_notpayin.F_ac_br_no===[%.12s]\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
  vtcp_log("[%s][%d]重新获取后:hv_notpayin.F_ac_br_no===[%.12s]\n",__FILE__,__LINE__,hv_notpayin->F_ac_br_no);
  return 0;
  
}


/*************************************************************************
* 函数名称：int  lv_Ins_Notpay_record(HV_NOTPAY_AREA *hv_notpayin,       *
*    struct hv_fzf_c *hv_fzf,LV_NOTPAY_ADD *lv_addinfo)       *
*                                                                        *
* 功能描述：根据前台输入的内容插入hv_fzf表记录 (是插入不是更             *
*           往hv_fzfhtr表中插入一条记录            *
* 注意    : 插入之前对hv_fzf.cmtno字段赋值  必须!!!                      *
*                                                                        *
* 返回值： 0-成功 1-失败                                                 * 
**************************************************************************/
int lv_Ins_Notpay_record(HV_NOTPAY_AREA *hv_notpayin,struct hv_fzf_c *hv_fzf,LV_NOTPAY_ADD *lv_addinfo)
{
  vtcp_log("[%s][%d]开始往hv_fzf/hv_fzfhtr表中插入记录\n",__FILE__,__LINE__);
  /****如果由于交易的不同需要对插入的字段进行更改，将更改放到具体交易中改动 ，改动hv_notpayin就可以了**/
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
     sprintf( acErrMsg,"得到支付交易序号错误![%s][%d]",__FILE__,__LINE__);
     WRITEMSG
     return (-1);
  }
  vtcp_log("[%s][%d]iOrderno=[%d]\n",__FILE__,__LINE__,iOrderno);

  sprintf(hv_notpayin->F_orderno,"%08ld",iOrderno);
  

  vtcp_log("[%s][%d]开始进行F->T操作!\n",__FILE__,__LINE__);
  ret = iLvfFrontToTable(hv_notpayin);
   if(ret)
   {
       sprintf(acErrMsg,"执行函数F->T错误!\n");
       WRITEMSG
       strcpy(g_pub_tx.reply,"P015");
   }
   vtcp_log("[%s][%d]hv_notpay->T_receco=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_receco);
   vtcp_log("[%s][%d]!!!!!req_orderno=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_req_orderno);
   vtcp_log("[%s][%d]!!!!!req_wtdate=[%s] \n",__FILE__,__LINE__,hv_notpayin->T_req_date);
   vtcp_log("[%s][%d]!!!!!req_br_no=[%s]  \n",__FILE__,__LINE__,hv_notpayin->T_req_br_no);
   vtcp_log("[%s][%d]!!!!!req_content=[%s]\n",__FILE__,__LINE__,hv_notpayin->T_req_content);
   vtcp_log("[%s][%d]F->T操作 成功!\n",__FILE__,__LINE__);
  
   vtcp_log("[%s][%d]开始对hv_fzf结构体赋值!\n",__FILE__,__LINE__);
   ret = iLvNotPayin_Copy(hv_notpayin,hv_fzf,lv_addinfo);
   if(ret)
   {
           sprintf(acErrMsg,"执行函数 iLvNotPayin_Cop错误!\n");
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
   vtcp_log("[%s][%d]对hv_fzf结构体赋值 成功!\n",__FILE__,__LINE__);
   
   vtcp_log("[%s][%d]开始插入非支付表!\n",__FILE__,__LINE__);
   /*****设置一下状态******/
   ret = iLvJudge_Change_notpay_stat(hv_fzf,hv_fzf->cmtno);
   if(ret)
   {
     vtcp_log("[%s][%d]判断和设置原非支付记录状态错误!\n",__FILE__,__LINE__);
     strcpy(g_pub_tx.reply,"P052");
     return (-1);
   }
   
   vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
   vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
   vtcp_log("[%s][%d]hv_fzf.stat==[%c]\n",__FILE__,__LINE__,hv_fzf->hv_fzf_sts[0]);
   Hv_fzf_Debug(hv_fzf);
   vtcp_log("[%s][%d]打印完毕!\n",__FILE__,__LINE__);
   ret = Hv_fzf_Ins(*hv_fzf,g_pub_tx.reply);
   if(ret)
   {
           sprintf(acErrMsg,"插入数据表hv_fzf错误!ret==[%d]\n",ret);
           WRITEMSG
           strcpy(g_pub_tx.reply,"HV01");
           return (-1);
   }
   vtcp_log("[%s][%d]hv_fzf->res__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->res_br_no);
   vtcp_log("[%s][%d]hv_fzf->req__br_no==[%s]\n",__FILE__,__LINE__,hv_fzf->req_br_no);
   vtcp_log("[%s][%d]hv_fzf->o_orderno==[%s]\n",__FILE__,__LINE__,hv_fzf->o_orderno);
   vtcp_log("[%s][%d]插入hv_fzf成功!\n",__FILE__,__LINE__);
   
   vtcp_log("[%s][%d]开始插入hv_fzfhtr\n",__FILE__,__LINE__);

   iLvFzfToFzfhtr(hv_fzf,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr的拷贝***/

   ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
   if(ret)
   {
           sprintf(acErrMsg,"插入数据表hv_fzfhtr错误!ret==[%d]\n",ret);
           WRITEMSG
           strcpy(g_pub_tx.reply,"HV01");
           return (-1);
   }
   
   vtcp_log("[%s][%d]插入hv_fzf/hv_fzfhtr成功!\n",__FILE__,__LINE__);
   
   return 0;
}



/****************************************************************************/
/* 函数名称: iLvfFrontToTable(HV_NOTPAY_AREA * hv_notpay)                   */
/* 功能描述: 从NOTPAY的F_字段赋值给T字段                                    */
/* 创建日期: 2006-8-19 19:12                                                */
/* 作者:     xyy                                                            */
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
  
  
  
  get_fd_data("0060",hv_notpay->T_tx_time);/* 交易时间*/
  get_fd_data("0040",hv_notpay->T_trace_no);/* 流水号*/
  vtcp_log("[%s][%d]hv_notpay->T_tx_time=[%s]",__FILE__,__LINE__,hv_notpay->T_tx_time);
  vtcp_log("[%s][%d]hv_notpay->T_trace_no=[%s]",__FILE__,__LINE__,hv_notpay->T_trace_no);
  hv_notpay->T_otype[0] = hv_notpay->F_otype[0];/*0大额 1小额*/
  vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay->T_otype[0]);
  hv_notpay->T_lw_ind[0] = hv_notpay->F_lw_ind[0]; /* 来往标识1-往2来 */
  hv_notpay->T_o_tx_type[0] = hv_notpay->F_o_tx_type[0];/* 原交易类型 */
  vtcp_log("[%s,%d]T_o_tx_type===[%c]\n",__FILE__,__LINE__,hv_notpay->T_o_tx_type[0]);
  hv_notpay->T_respcode[0] = hv_notpay->F_respcode[0];/* 退回/撤销应答,0, 可以 1，不可以 */
  vtcp_log("[%s][%d]hv_notpay->F_respcode=[%c]",__FILE__,__LINE__,hv_notpay->F_respcode[0]);
  memcpy(hv_notpay->T_orderno,hv_notpay->F_orderno ,sizeof(hv_notpay->T_orderno));/*交易序号*/
  memcpy(hv_notpay->T_tx_date,hv_notpay->F_tx_date ,sizeof(hv_notpay->T_tx_date));/*交易日期*/
  memcpy(hv_notpay->T_wt_date,hv_notpay->F_wt_date ,sizeof(hv_notpay->T_wt_date));/*委托日期*/
  memcpy(hv_notpay->T_br_no  ,hv_notpay->F_tx_br_no ,sizeof(hv_notpay->T_br_no));/*交易机构*/
  GetOr_br_noByBr_no(hv_notpay->T_br_no  ,hv_notpay->T_or_br_no);  /* 交易行号 */
  memcpy(cCmtno,hv_notpay->F_cmtno,sizeof(cCmtno)-1);
  memcpy(hv_notpay->T_cmtno ,hv_notpay->F_cmtno ,sizeof(hv_notpay->T_cmtno));/* 交易类型 */
  vtcp_log("[%s][%d]hv_notpay->T_or_br_no=[%s]",__FILE__,__LINE__,hv_notpay->T_or_br_no);
  /*得到发报中心和发送行清算中心*/
  iGet_full_hv_no(hv_notpay->T_or_br_no,hv_notpay->T_sendco,hv_notpay->T_pay_qs_no);
  
  vtcp_log("[%s][%d]hv_notpay->T_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpay->T_cash_qs_no);
  /* 原委托日期 */
  memcpy(hv_notpay->T_o_wt_date  ,hv_notpay->F_o_wt_date  ,sizeof(hv_notpay->T_o_wt_date ));
  /* 原发起行行号 */
  memcpy(hv_notpay->T_o_or_br_no ,hv_notpay->F_o_or_br_no ,sizeof(hv_notpay->T_o_or_br_no));
  /***Modified by SSH,2006.8.28,如果接行4位表示是收报中心****/
  pub_base_full_anti_zero(hv_notpay->F_ac_br_no,sizeof(hv_notpay->F_ac_br_no));
  if(hv_notpay->F_ac_br_no[5]=='\0' && hv_notpay->F_ac_br_no[0]!=0x00)
  {  
    vtcp_log("[%s][%d] dhcc 这是到收报中心的自由格式!hv_notpay->F_ac_br_no==[%s]\n",__FILE__,__LINE__,hv_notpay->F_ac_br_no);
    memset(hv_notpay->T_ac_br_no  ,'\0',sizeof(hv_notpay->T_ac_br_no));/* 接收行行号 */
    memcpy(hv_notpay->T_receco,hv_notpay->F_ac_br_no,sizeof(hv_notpay->T_receco));
    vtcp_log("[%s][%d]hv_notpay.T_receco===[%s]\n",__FILE__,__LINE__,hv_notpay->T_receco);
  }else
  {
    vtcp_log("[%s][%d]hv_notpay->T_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpay->T_cash_qs_no);
    /* 接收行行号 */
    memcpy(hv_notpay->T_ac_br_no  ,hv_notpay->F_ac_br_no   ,sizeof(hv_notpay->T_ac_br_no));
    /*得到收中心和接收行清算中心*/
    iGet_full_hv_no(hv_notpay->T_ac_br_no,hv_notpay->T_receco,hv_notpay->T_cash_qs_no);
  }
  vtcp_log("[%s][%d]hv_notpay->T_cash_qs_no=[%s]",__FILE__,__LINE__,hv_notpay->T_cash_qs_no);
  /* 原接收行行号 */
  memcpy(hv_notpay->T_o_ac_br_no ,hv_notpay->F_o_ac_br_no ,sizeof(hv_notpay->T_o_ac_br_no));
  /* 原支付交易序号 */
  memcpy(hv_notpay->T_o_payin_orderno ,hv_notpay->F_o_payin_orderno ,sizeof(hv_notpay->T_o_payin_orderno ));
  /* 原币种 */
  memcpy(hv_notpay->T_o_cur_no   ,hv_notpay->F_o_cur_no   ,sizeof(hv_notpay->T_o_cur_no));
  /* 原交易金额 */
  memcpy(hv_notpay->T_o_tx_amt   ,hv_notpay->F_o_txamt    ,sizeof(hv_notpay->T_o_tx_amt));
  /* 原业务类型 */
  memcpy(hv_notpay->T_o_cmtno    ,hv_notpay->F_o_cmtno    ,sizeof(hv_notpay->T_o_cmtno));
  vtcp_log("[%s][%d]F_content=[%s]",__FILE__,__LINE__,hv_notpay->F_content);
  vtcp_log("[%s][%d]F_cmtno=[%s]",__FILE__,__LINE__,hv_notpay->F_cmtno);
  vtcp_log("[%s][%d]cCmtno=[%s]",__FILE__,__LINE__,cCmtno);
  /*****对申请类和应答类分别处理*****//**by liuyue 20060913***/
  if(!(memcmp(cCmtno,"301",3)==0 ||memcmp(cCmtno,"303",3)==0||memcmp(cCmtno,"319",3)==0
    ||memcmp(cCmtno,"321",3)==0 ||memcmp(cCmtno,"323",3)==0||memcmp(cCmtno,"327",3)==0||memcmp(cCmtno,"302",3)==0 ||memcmp(cCmtno,"320",3)==0 ||memcmp(cCmtno,"322",3)==0 
    ||memcmp(cCmtno,"324",3)==0 ||memcmp(cCmtno,"328",3)==0 ))
  {
    vtcp_log("[%s][%d]错误的CMTNO编号!cmtno==[%.3s]\n",__FILE__,__LINE__,cCmtno);
    strcpy(g_pub_tx.reply,"P065");
    return (-1);
  }
  if(memcmp(cCmtno,"301",3)==0 ||memcmp(cCmtno,"303",3)==0||memcmp(cCmtno,"319",3)==0
     ||memcmp(cCmtno,"321",3)==0 ||memcmp(cCmtno,"323",3)==0||memcmp(cCmtno,"327",3)==0)
  {
    /* 申请交易序号 */
    memcpy(hv_notpay->T_req_orderno,hv_notpay->F_orderno,sizeof(hv_notpay->T_req_orderno));
    /* 申请日期 */
    memcpy(hv_notpay->T_req_date,hv_notpay->F_wt_date ,sizeof(hv_notpay->T_req_date));
    /* 申请机构 */
    vtcp_log("[%s][%d]hv_notpay->F_or_br_no=[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
    memcpy(hv_notpay->T_req_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_req_br_no)); 
    /* 应答机构 */
    memcpy(hv_notpay->T_res_br_no,hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_res_br_no));
    vtcp_log("[%s][%d]hv_notpay->T_res_br_no=[%.12s]\n",__FILE__,__LINE__,hv_notpay->T_res_br_no);
    /* 申请内容 */
    vtcp_log("[%s][%d]hv_notpay->T_req_br_no=[%.12s]\n",__FILE__,__LINE__,hv_notpay->T_req_br_no);
    memcpy(hv_notpay->T_req_content,hv_notpay->F_content,sizeof(hv_notpay->T_req_content));
    vtcp_log("[%s][%d]T_req_content=[%s]",__FILE__,__LINE__,hv_notpay->T_req_content);
}
  if(memcmp(cCmtno,"302",3)==0 ||memcmp(cCmtno,"320",3)==0 ||memcmp(cCmtno,"322",3)==0 
  ||memcmp(cCmtno,"324",3)==0 ||memcmp(cCmtno,"328",3)==0  )
  {  /* 应答交易序号 */
    memcpy(hv_notpay->T_res_orderno,hv_notpay->F_orderno,sizeof(hv_notpay->T_res_orderno));
    /* 应答日期 */
    memcpy(hv_notpay->T_res_date,hv_notpay->F_wt_date ,sizeof(hv_notpay->T_res_date));
    /* 应答机构 */
    memcpy(hv_notpay->T_res_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_res_br_no));
    /* 申请内容 */
    memcpy(hv_notpay->T_res_content,hv_notpay->F_content,sizeof(hv_notpay->T_res_content));
    vtcp_log("[%s][%d]T_res_content=[%s]",__FILE__,__LINE__,hv_notpay->T_res_content);
  }
  vtcp_log("[%s][%d]T_req_content=[%s]",__FILE__,__LINE__,hv_notpay->T_req_content);
  vtcp_log("[%s][%d]T_res_content=[%s]",__FILE__,__LINE__,hv_notpay->T_res_content);
  hv_notpay->T_o_tx_type[0] = hv_notpay->F_o_tx_type[0];/**原业务种类**/
  memcpy(hv_notpay->T_refno ,hv_notpay->F_refno  ,sizeof(hv_notpay->T_refno));/* 报文参考号 */
  memcpy(hv_notpay->T_tel    ,hv_notpay->F_tel    ,sizeof(hv_notpay->T_tel));/* 操作员号 */
  memcpy(hv_notpay->T_send_tel,hv_notpay->F_send_tel,sizeof(hv_notpay->F_send_tel));
  memcpy(hv_notpay->T_hv_brf ,hv_notpay->F_hv_brf,sizeof(hv_notpay->T_hv_brf)); /* 备注 */
  
  

  if(memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
  {/* 01录入 */
    hv_notpay->T_hv_fzf_sts[0]=SEND_REGIST;
  }
  else if(memcmp(hv_notpay->F_opcd,"30",sizeof(hv_notpay->F_opcd))==0)
  {/* 03发送 */
    hv_notpay->T_hv_fzf_sts[0]=SEND_ACCT_WAIT;
  }
  else if(memcmp(hv_notpay->F_opcd,"60",sizeof(hv_notpay->F_opcd))==0)
  {/* 04接收 */
    if (!memcmp(g_pub_tx.reply,"0000",4))
    {
      hv_notpay->T_hv_fzf_sts[0]=RECEIVE_ACCT_QS;
    }
    else
    {
      /* 记账失败了，要先判断接收机构是否存在，存在挂支行，不存在挂清算 */
      hv_notpay->T_hv_fzf_sts[0]=RECEIVE_CUSTNAME_ERR;
    }
  }
  else if(memcmp(hv_notpay->F_opcd,"50",sizeof(hv_notpay->F_opcd))==0)
  {/* 05应答 */
    hv_notpay->T_hv_fzf_sts[0]=SEND_ACCT_QS;
  }
  else
  {
    vtcp_log("[%s][%d]非支付交易好象没有此种交易类型啊！F_opcd=[%s]",__FILE__,__LINE__,hv_notpay->F_opcd);
    sprintf(acErrMsg,"非支付交易好象没有此种交易类型啊！F_opcd!\n");
    WRITEMSG
    return 1;
  }
  vtcp_log("[%s][%d]cmtno=[%.3s]",__FILE__,__LINE__,hv_notpay->T_cmtno);

  
  
  vtcp_log("[%s][%d]F_opcd=[%s]",__FILE__,__LINE__,hv_notpay->F_opcd);
  vtcp_log("[%s][%d]fzf_sts=[%c]",__FILE__,__LINE__,hv_notpay->T_hv_fzf_sts[0]);
  return 0;
}


/****************************************************************************/
/* 函数名称: iLvTistoNotpay(HV_NOTPAY_AREA * hv_notpay)                     */
/* 功能描述: 从前台得到数据到F_字段                                         */
/* 创建日期: 2006-8-19 15:58                                                */
/* 作者:     xyy                                                           */
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
  
  vtcp_log("123域=[%s]",(char*)&fd123.ac_br_no); 
  
  vtcp_log("123域=[%s]",(char*)&fd123.cmtno); 
  
  memcpy(hv_notpay->F_cmtno,fd123.cmtno,sizeof(hv_notpay->F_cmtno));
  vtcp_log("前台传来的cmtno=[%.3s]",hv_notpay->F_cmtno); 
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  memcpy(hv_notpay->F_o_cmtno,fd123.o_cmtno,sizeof(hv_notpay->F_o_cmtno));
  vtcp_log("前台传来的o_cmtno=[%.3s]",hv_notpay->F_o_cmtno); 
  
  
  vtcp_log("前台传来的pay_qs_no 不要!!!\n");
  
  
  memcpy(hv_notpay->F_or_br_no,fd123.or_br_no,sizeof(hv_notpay->F_or_br_no));
  vtcp_log("前台传来的 发起行/原接受行 F_or_br_no=[%.12s]",hv_notpay->F_or_br_no); 
  
  
  
  vtcp_log("前台传来的sendco 不要!!!\n");
  vtcp_log("前台传来的pay_opn_br_no 不要!!!\n");
  vtcp_log("前台传来的pay_ac_no 不要!!!\n");
  vtcp_log("前台传来的  pay_name  不要!!!\n");
  vtcp_log("前台传来的   pay_addr 不要!!!\n");
  vtcp_log("前台传来的  cash_qs_no 不要!!!\n");
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  memcpy(hv_notpay->F_ac_br_no,fd123.ac_br_no,sizeof(hv_notpay->F_ac_br_no));
  vtcp_log("前台传来的 接受行 F_ac_br_no=[%.12s]",hv_notpay->F_ac_br_no); 
  
  
  vtcp_log("前台传来的  receco =[%.4s]!!!\n",fd123.receco);
  vtcp_log("前台传来的  cash_opn_br_no 不要!!!\n");
  vtcp_log("前台传来的  cash_ac_no 不要!!!\n");
  vtcp_log("前台传来的  cash_name 不要!!!\n");
  vtcp_log("前台传来的  cash_addr 不要!!!\n");
  vtcp_log("前台传来的  yw_type 不要!!!\n");
  
  
  memcpy(hv_notpay->F_orderno ,fd123.orderno ,sizeof(hv_notpay->F_orderno ));
  vtcp_log("前台传来的   交易序号 F_orderno =[%.8s]",hv_notpay->F_orderno); 
  
  
  memcpy(hv_notpay->F_hv_brf,fd123.brf,sizeof(hv_notpay->F_hv_brf));
  vtcp_log("前台传来的   附言 F_hv_brf =[%.60s]",hv_notpay->F_hv_brf); 
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  vtcp_log("前台传来的   利率 rate 不要!!!\n");
  vtcp_log("前台传来的   利率期限ratelimit 不要!!!\n");
  
  
  memcpy(hv_notpay->F_o_payin_orderno ,fd123.o_orderno ,sizeof(hv_notpay->F_o_payin_orderno ));
  vtcp_log("前台传来的 交易序号 F_o_payin_orderno =[%.8s]",hv_notpay->F_o_payin_orderno); 
  
  
  memcpy(hv_notpay->F_resp1,fd123.procode,sizeof(hv_notpay->F_resp1));
  vtcp_log("前台传来的 处理号码 hv_notpay->F_resp1[%.8s]",hv_notpay->F_resp1);
  
  
  
  vtcp_log("前台传来的  查复日期cr_date=[%.8s]!!!\n",fd123.cr_date);
  vtcp_log("前台传来的  查复行号cr_br_no=[%.12s]!!!\n",fd123.cr_br_no);
  vtcp_log("前台传来的  查复书号cr_tx_num =[%.8s]!!!\n",fd123.cr_tx_num );
  vtcp_log("前台传来的  查询行号qr_br_no=[%.12s]!!!\n",fd123.qr_br_no);
  vtcp_log("前台传来的  查询书号qr_tx_num=[%.8s]!!!\n",fd123.qr_tx_num);
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  hv_notpay->F_o_tx_type[0] = fd123.tx_type[0];
  vtcp_log("前台传来的  原业务类型 tx_type===[%c]\n",hv_notpay->F_o_tx_type[0]);
  
  
  memcpy(hv_notpay->F_content,fd123.notpay_content,sizeof(hv_notpay->F_content)); 
  vtcp_log("前台传来的  附言内容 F_content===[%.255s]\n",hv_notpay->F_content);
  
  /*
  memcpy(hv_notpay->F_o_notpay_orderno,fd123.notpay_orderno,sizeof(hv_notpay->F_o_notpay_orderno));
  vtcp_log("前台传来的  非支付交易序号 F_o_notpay_orderno===[%.8s]\n",hv_notpay->F_o_notpay_orderno);*/
  
  /******************
  memcpy(hv_notpay->F_o_notpay_orderno,"00000",5);
  memcpy(hv_notpay->F_o_notpay_orderno+5,fd123.o_notpay_orderno,sizeof(fd123.o_notpay_orderno));
  vtcp_log("前台传来的  原非支付交易序号 F_o_notpay_orderno===[%.8s]\n",hv_notpay->F_o_notpay_orderno);
  ************/
  memcpy(hv_notpay->F_o_notpay_orderno,fd123.qr_tx_num,sizeof(fd123.qr_tx_num));
  vtcp_log("前台传来的  原非支付交易序号 F_o_notpay_orderno===[%.8s]\n",hv_notpay->F_o_notpay_orderno);
  
  
  hv_notpay->F_respcode[0] = fd123.resp[0];
  vtcp_log("前台传来的 撤消或退回应答 hv_notpay->F_respcode[0]=[%c]",hv_notpay->F_respcode[0]); 
  
  
  vtcp_log("前台传来的  优先级别 operlevel=[%c]!!!\n",fd123.operlevel[0]);
  
  
  vtcp_log("前台传来的  处理状态 proc_sts=[%c]!!!\n",fd123.proc_sts[0]);
  
  
  memcpy(hv_notpay->F_opcd,fd123.opcd,sizeof(hv_notpay->F_opcd));
  vtcp_log("前台传来的 操作代码 opcd=[%.2s]",hv_notpay->F_opcd); 
   
  hv_notpay->F_lw_ind[0]=fd123.lw_ind[0];
  vtcp_log("前台传来的  往来标志F_lw_ind[0]=[%c]",fd123.lw_ind[0]); 
  
  
  hv_notpay->F_hp_add_pwd[0] = fd123.hp_add_pwd[0];
  vtcp_log("前台传来的   汇票是否核押 F_hp_add_pwd[0]=[%c]",hv_notpay->F_hp_add_pwd[0]); 
  
  vtcp_log("前台传来的   包序号= packid [%.8s]!!!\n",fd123.packid);
  
  
  vtcp_log("前台传来的   原包序号= o_packid [%.8s]!!!\n",fd123.o_packid);
  
  vtcp_log("前台传来的   包日期= pack_date [%.8s]!!!\n",fd123.pack_date);
  
  vtcp_log("前台传来的   原包日期= o_pack_date [%.8s]!!!\n",fd123.o_pack_date);
  
  vtcp_log("前台传来的   业务类型编号 = txnum [%.5s]!!!\n",fd123.txnum);
  
  vtcp_log("前台传来的   原业务类型编号= o_txnum [%.5s]!!!\n",fd123.o_txnum);
  
  vtcp_log("前台传来的   小额业务种类号,2位编号或者12位描述 = lv_yw_ind [%.12s]!!!\n",fd123.lv_yw_ind);
  
  vtcp_log("前台传来的   应答日期 = resp_date [%.8s]!!!\n",fd123.resp_date);
  
  vtcp_log("前台传来的   业务回执状态 = rcpstat [%.2s]!!!\n",fd123.rcpstat);
  
  vtcp_log("前台传来的   ccpc交易处理代码 = resp2 [%.8s]!!!\n",fd123.resp2);
  
  vtcp_log("前台传来的   退汇原因= retcode [%.2s]!!!\n",fd123.retcode);
  
  vtcp_log("前台传来的   明细序号 = ywdtlid [%.8s]!!!\n",fd123.ywdtlid);
  
  vtcp_log("前台传来的   实时业务付款人帐户类型 = actype [%c]!!!\n",fd123.actype);
  
  vtcp_log("前台传来的   申请类型 0＝单笔  1＝整包 = reqtype [%c]!!!\n",fd123.reqtype[0]);
  
  vtcp_log("前台传来的   对方手续费 = df_chrg_amt [%.16s]!!!\n",fd123.df_chrg_amt);
  
  vtcp_log("前台传来的   多方协议号= prot_no [%.60s]!!!\n",fd123.prot_no);
  
  vtcp_log("前台传来的   汇票号码 = po_no [%.20s]!!!\n",fd123.po_no);
  
  vtcp_log("前台传来的   汇票类型0可转 1 不可转 2现金 = po_type [%c]!!!\n",fd123.po_type[0]);
  
  vtcp_log("前台传来的   大小额标志 0大额，1小额 = otype  [%c]!!!\n",fd123.otype[0]);
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
   
   
   
  vtcp_log("[%s][%d]到这里获取到了所有的FD123域里面的内容!\n",__FILE__,__LINE__);
  
  
  get_fd_data("0030",hv_notpay->F_tx_br_no);
  vtcp_log("前台传来的交易机构 F_tx_br_no=[%.5s]",hv_notpay->F_tx_br_no); 
  
  
  
  
  memcpy(hv_notpay->F_wt_date,pcLvGetDay(),sizeof(hv_notpay->F_wt_date));
  /********委托日期必须用小额日期,但是交易日期仍旧为综合系统日期*******/
  vtcp_log("前台传来的 委托日期 wt_date=[%.8s]",hv_notpay->F_wt_date); 
  
  
  get_fd_data("0440",hv_notpay->F_tx_date);
  vtcp_log("前台传来的 交易日期 F_tx_date=[%.8s]",hv_notpay->F_tx_date); 
  
  
  get_fd_data("0210",cCur_no);
  vCur_dhcc_mbfe(cCur_no,hv_notpay->F_o_cur_no);
  vtcp_log("前台传来的 币种 F_o_cur_no=[%.3s]",hv_notpay->F_o_cur_no); 
  
  
  get_fd_data("0390",hv_notpay->F_o_txamt);
  vtcp_log("前台传来的  交易金额txamt=[%.16s]",hv_notpay->F_o_txamt); 
  
  
  
  
  get_fd_data("0460",hv_notpay->F_o_fzf_date);/* 原非支付交易日期,查复中用到的查询日期 */    
  vtcp_log("前台传来的 原非支付交易日期 F_o_fzf_date=[%.8s]",hv_notpay->F_o_fzf_date); 
  
  
  hv_notpay->F_otype[0] = fd123.otype[0];
  vtcp_log("前台传来的 选择大小额 0 大额 1 小额 hv_notpay->F_otype[0]=[%c]",hv_notpay->F_otype[0]); 
  
  
  get_fd_data("0450",hv_notpay->F_o_wt_date);
  vtcp_log("前台传来的  原委托日期o_wt_date=[%.8s]",hv_notpay->F_o_wt_date);
  
  
  if(memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
  {
    iGetHvRefno(HV_QS_BR_NO,hv_notpay->F_refno);
  }
  vtcp_log("从前台获取的报文参考好 hv_notpay->F_refno=[%s]",hv_notpay->F_refno);
  vtcp_log("[%s][%d]ORBRNO==[%.12s]\n",__FILE__,__LINE__,hv_notpay->F_or_br_no);
  
  
  
  /****************要有什么特别处理 都到这里来吧!**********************/
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  memcpy(cOr_br_no,hv_notpay->F_or_br_no,sizeof(cOr_br_no)-1);
  zip_space(cOr_br_no);
  vtcp_log("从前台获取的本行行号(生成的)F_or_br_no=[%.12s]\n",hv_notpay->F_or_br_no);
  if(cOr_br_no[0]==0x00)
  {
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    GetOr_br_noByBr_no(hv_notpay->F_tx_br_no,hv_notpay->F_or_br_no);
    vtcp_log("从前台获取的本行行号(生成的)F_or_br_no=[%.12s]\n",hv_notpay->F_or_br_no);
  }
  vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
  
  
  if(memcmp(hv_notpay->F_opcd,"10",sizeof(hv_notpay->F_opcd))==0)
  {
    get_fd_data("0070",hv_notpay->F_tel);
    vtcp_log("前台传来的 操作员号tel=[%s]",hv_notpay->F_tel); 
  }
  else if(memcmp(hv_notpay->F_opcd,"40",sizeof(hv_notpay->F_opcd))==0)
  {
    get_fd_data("0070",hv_notpay->F_send_tel);
    vtcp_log("前台传来的 操作员号 tel=[%s]",hv_notpay->F_send_tel); 
  }
  
  
  if(memcmp(cCmtno,"301",3)==0 )/**需要时再添加**/
  {
    /*原支付交易发起行*/
    memcpy(hv_notpay->F_o_or_br_no,fd123.ac_br_no,sizeof(hv_notpay->F_o_or_br_no));
    /*原支付交易接收行*/
    memcpy(hv_notpay->F_o_ac_br_no,fd123.or_br_no,sizeof(hv_notpay->F_o_ac_br_no));
  }
  vtcp_log("[%s][%d]执行函数 iLvTistoNotpay 成功!\n",__FILE__,__LINE__);
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
    vtcp_log("[%s][%d]获取交易序号错误!\n",__FILE__,__LINE__);
    return (-1);
  }
  
  
  iOrderno = apatoi(cOrderno,sizeof(cOrderno)-1);
  
  vtcp_log("[%s][%d]iOrderno=[%lf]\n",__FILE__,__LINE__,iOrderno);
  
  return iOrderno;
}    
                 
