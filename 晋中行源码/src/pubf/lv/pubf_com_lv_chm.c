/***************************************************************************/
/* �ļ�����: pubf_com_lv_chm.c                                             */
/* ��������: С��֧������������������                                    */
/* ��������: 2006.09.12  .                                                 */
/* ����:     ChenMing                                                      */
/* ˵��:                                                                   */
/***************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lv_define.h"
#include "hv_define.h"
#include "lv_tito.h"
#include "lv_pub.h"
#include "hv_orno_brno_c.h"
#include "lv_newxyh_c.h"
#include "lv_addit_c.h"
#include "lv_sysctl_c.h"
#include "seqctl_c.h"
#include "lv_fd123.h"
#include "public.h"
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "lv_c254r_c.h"
#include "lv_add.h"

char* pcLvGetDay();
char* pcPayinRetdtl(PAY_IN_AREA *wp_payin);
char * pcLvDesPswdCode(char *code);
char* pcLvGetRecvno();

int pubf_lv_print_NOTPAY_IN_AREA(NOTPAY_IN_AREA * pNotpay_in)
{
  
     vtcp_log("pNotpay_in->optype =[%.2s]",    pNotpay_in->optype);
     vtcp_log("pNotpay_in->pkgno  =[%.3s]",    pNotpay_in->pkgno);
     vtcp_log("pNotpay_in->cmtno  =[%.3s]",    pNotpay_in->cmtno);     
     vtcp_log("pNotpay_in->orderno=[%.8s]",    pNotpay_in->orderno);
     vtcp_log("pNotpay_in->txnum  =[%.5s]",    pNotpay_in->txnum);
     vtcp_log("pNotpay_in->acbrno =[%.12s]",   pNotpay_in->acbrno);
     
     vtcp_log("pNotpay_in->rettype =[%.c]",    pNotpay_in->rettype);
     vtcp_log("pNotpay_in->respcode=[%c]",     pNotpay_in->respcode);
     vtcp_log("pNotpay_in->opackday=[%.8s]",   pNotpay_in->opackday);
     vtcp_log("pNotpay_in->opkgno  =[%.3s]",   pNotpay_in->opkgno);
     vtcp_log("pNotpay_in->opackid =[%.8s]",   pNotpay_in->opackid);
     vtcp_log("pNotpay_in->otxnum  =[%.5s]",   pNotpay_in->otxnum);
          
     vtcp_log("pNotpay_in->owtday  =[%.8s]",   pNotpay_in->owtday);
     vtcp_log("pNotpay_in->oorderno=[%.8s]",   pNotpay_in->oorderno);
     vtcp_log("pNotpay_in->oorbrno =[%.12s]",  pNotpay_in->oorbrno );
     vtcp_log("pNotpay_in->oacbrno =[%.12s]",  pNotpay_in->oacbrno );
     vtcp_log("pNotpay_in->otxtype =[%.c]",    pNotpay_in->otxtype );
     vtcp_log("pNotpay_in->cour    =[%.3s]",   pNotpay_in->cour    );
     vtcp_log("pNotpay_in->otxamt  =[%.15s]",  pNotpay_in->otxamt  );
     vtcp_log("pNotpay_in->content =[%.255s]", pNotpay_in->content );
     vtcp_log("pNotpay_in->refno   =[%.20s]",  pNotpay_in->refno   );

     vtcp_log("pNotpay_in->unit      =[%.8s]",  pNotpay_in->unit      );
     vtcp_log("pNotpay_in->belongcyc =[%.8s]",  pNotpay_in->belongcyc );
     vtcp_log("pNotpay_in->sftype    =[%.12s]", pNotpay_in->sftype    );
     vtcp_log("pNotpay_in->pdate     =[%.12s]", pNotpay_in->pdate     );
     vtcp_log("pNotpay_in->pnum      =[%.c]",   pNotpay_in->pnum      );
     vtcp_log("pNotpay_in->paybrname =[%.3s]",  pNotpay_in->paybrname );
     vtcp_log("pNotpay_in->actno     =[%.15s]", pNotpay_in->actno     );
     vtcp_log("pNotpay_in->name      =[%.255s]",pNotpay_in->name      );
     vtcp_log("pNotpay_in->reason    =[%.20s]", pNotpay_in->reason    );
                                           
     vtcp_log("pNotpay_in->bsnum    =[%.2s]",   pNotpay_in->bsnum   );
     vtcp_log("pNotpay_in->passwd   =[%.60s]",  pNotpay_in->passwd  );
     vtcp_log("pNotpay_in->datalen  =[%.8s]",   pNotpay_in->datalen );
     vtcp_log("pNotpay_in->acttype  =[%.c]",    pNotpay_in->acttype );
     vtcp_log("pNotpay_in->pswdcd   =[%.c]",    pNotpay_in->pswdcd  );
     vtcp_log("pNotpay_in->pascode  =[%.128s]", pNotpay_in->pascode );
     vtcp_log("pNotpay_in->qtype    =[%.c]",    pNotpay_in->qtype   );


     vtcp_log("pNotpay_in->orbrno      =[%.12s]",  pNotpay_in->orbrno     );
     vtcp_log("pNotpay_in->payqsactno  =[%.12s]",  pNotpay_in->payqsactno );
     vtcp_log("pNotpay_in->sendco      =[%.4s]",   pNotpay_in->sendco     );
     vtcp_log("pNotpay_in->cashqsactno =[%.12s]",  pNotpay_in->cashqsactno);
     vtcp_log("pNotpay_in->receco      =[%.4s]",   pNotpay_in->receco     );
     vtcp_log("pNotpay_in->wssrno      =[%.6s]",   pNotpay_in->wssrno     );
     vtcp_log("pNotpay_in->txday       =[%.8s]",   pNotpay_in->txday      );
     vtcp_log("pNotpay_in->stat        =[%.c]",    pNotpay_in->stat       );
     vtcp_log("pNotpay_in->datatype    =[%.c]",    pNotpay_in->datatype   );
                                              
     vtcp_log("pNotpay_in->addid     =[%.8s]",  pNotpay_in->addid    );
     vtcp_log("pNotpay_in->qorderno  =[%.8s]",  pNotpay_in->qorderno );
     vtcp_log("pNotpay_in->qwtday    =[%.8s]",   pNotpay_in->qwtday   );
     vtcp_log("pNotpay_in->qorbrno   =[%.12s]",  pNotpay_in->qorbrno  );
     vtcp_log("pNotpay_in->racbrno   =[%.12s]",   pNotpay_in->racbrno  );
     vtcp_log("pNotpay_in->rwtday    =[%.12s]",   pNotpay_in->rwtday   );
     vtcp_log("pNotpay_in->rorderno  =[%.12s]",   pNotpay_in->rorderno );
     vtcp_log("pNotpay_in->totamt    =[%.15s]",    pNotpay_in->totamt   );
     vtcp_log("pNotpay_in->totcnt    =[%.8s]",    pNotpay_in->totcnt   );
     return 0;
}


int pubf_lv_print_PAY_IN_AREA(PAY_IN_AREA * pPay_in_area)
{
  
    vtcp_log(" pPay_in_area->F_wtday =[%.8s] ",pPay_in_area->F_wtday);
    vtcp_log(" pPay_in_area->T_wtday =[%.8s] ",pPay_in_area->T_wtday);
    vtcp_log(" pPay_in_area->T_jzday =[%.8s] ",pPay_in_area->T_jzday);
    
    vtcp_log(" pPay_in_area->F_txamt =[%.15s] ",pPay_in_area->F_txamt);
    vtcp_log(" pPay_in_area->T_txamt =[%.15s] ",pPay_in_area->T_txamt);
    
    vtcp_log(" pPay_in_area->T_sendco =[%.4s] ",pPay_in_area->T_sendco);
    vtcp_log(" pPay_in_area->T_receco =[%.4s] ",pPay_in_area->T_receco);
    
    vtcp_log(" pPay_in_area->F_rcpstat =[%.2s] ",pPay_in_area->F_rcpstat);
    vtcp_log(" pPay_in_area->T_rcpstat =[%.2s] ",pPay_in_area->T_rcpstat);    
    vtcp_log(" pPay_in_area->F_resp1 =[%.8s] ",pPay_in_area->F_resp1);  
    vtcp_log(" pPay_in_area->T_resp1 =[%.8s] ",pPay_in_area->T_resp1);        
    vtcp_log(" pPay_in_area->F_resp2 =[%.8s] ",pPay_in_area->F_resp2);    
    vtcp_log(" pPay_in_area->T_resp2 =[%.8s] ",pPay_in_area->T_resp2);  
          
    vtcp_log(" pPay_in_area->T_brno =[%.7s] ",pPay_in_area->T_brno);

    vtcp_log(" pPay_in_area->T_inputno =[%.6s] ",pPay_in_area->T_inputno);  
    vtcp_log(" pPay_in_area->T_checkno =[%.6s] ",pPay_in_area->T_checkno);
    vtcp_log(" pPay_in_area->T_authno =[%.6s] ",pPay_in_area->T_authno);
    vtcp_log(" pPay_in_area->T_sendno =[%.6s] ",pPay_in_area->T_sendno);
    vtcp_log(" pPay_in_area->T_respno =[%.6s] ",pPay_in_area->T_respno);    
    vtcp_log(" pPay_in_area->T_recvno =[%.6s] ",pPay_in_area->T_recvno);
    vtcp_log(" pPay_in_area->T_dealno =[%.6s] ",pPay_in_area->T_dealno);
    vtcp_log(" pPay_in_area->T_inwssrno =[%.6s] ",pPay_in_area->T_inwssrno);    
    
    vtcp_log(" pPay_in_area->T_checkwssrno =[%.6s] ",pPay_in_area->T_checkwssrno);
    vtcp_log(" pPay_in_area->T_sendwssrno =[%.6s] ",pPay_in_area->T_sendwssrno);    
    vtcp_log(" pPay_in_area->T_respwssrno =[%.6s] ",pPay_in_area->T_respwssrno);
    vtcp_log(" pPay_in_area->T_recvwssrno =[%.6s] ",pPay_in_area->T_recvwssrno);
    vtcp_log(" pPay_in_area->T_dealwssrno =[%.6s] ",pPay_in_area->T_dealwssrno);    


    vtcp_log(" pPay_in_area->T_retcode =[%.2s] ",pPay_in_area->T_retcode);
    vtcp_log(" pPay_in_area->T_otxnum =[%.5s] ",pPay_in_area->T_otxnum);
    vtcp_log(" pPay_in_area->T_oorderno =[%.8s] ",pPay_in_area->T_oorderno);  

}


int notpayin_to_table(NOTPAY_IN_AREA *wp_notpayin)
{
  char  cCmtno[4];
  char  cPkgno[4];
  char  cBrno[6];
  char  cTime[7];
  char  cTlrno[5];
  char  cBakcode[13];
  char  cOrderno[9];
  char  cTempstr[1024];
  int     iPkgno=0;
  int  iCmtno=0;
  int ret=0;
  PAY_IN_AREA wd_payin;
            
  NOTPAY_IN_AREA  notpayin;
  struct lv_pkgreg_c    sLvPkgreg;

  memset(&sLvPkgreg, 0, sizeof(sLvPkgreg));
  memset(&notpayin, 0, sizeof(notpayin));
  memset(cCmtno, 0, sizeof(cCmtno));
  memset(cBrno, 0, sizeof(cBrno));
  memset(cTime, 0, sizeof(cTime));
  memset(cTlrno, 0, sizeof(cTlrno));
  memset(cPkgno, 0, sizeof(cPkgno));
  memset(cBakcode, 0, sizeof(cBakcode));
  memset(cOrderno, 0, sizeof(cOrderno));

  memcpy(&notpayin, wp_notpayin, sizeof(notpayin));
  memcpy(cCmtno, wp_notpayin->cmtno, sizeof(cCmtno)-1);
  memcpy(cPkgno, wp_notpayin->pkgno, sizeof(cPkgno)-1);

  iCmtno = atoi(cCmtno);
  iPkgno = atoi(cPkgno);

  /* ���׻��� */
  get_fd_data("0030",cBrno);
  /* ����ʱ�� */
  get_fd_data("0060",cTime);
  /* ����Ա */
  get_fd_data("0070",cTlrno);
  vtcp_log("%s,%d cashqsactno=[%.12s]",__FILE__,__LINE__,wp_notpayin->cashqsactno);
  vtcp_log("%s,%d cashqsactno=[%.12s]",__FILE__,__LINE__,wp_notpayin->cashqsactno);
  
  vtcp_log("[%s][%d]====cBrno======[%s]\n",__FILE__,__LINE__,cBrno);
  vtcp_log("[%s][%d]====cTlrno======[%s]\n",__FILE__,__LINE__,cTlrno);
  
  vtcp_log("[%s][%d]=========notpayin.oorbrno=[%s]\n",__FILE__,__LINE__,notpayin.oorbrno);
  if (lv_get_not_orient(wp_notpayin) == TX_RECEIVE)
  {
    vtcp_log("[%s][%d] notpayin_to_table_lz() ����С�����ʵǼǱ� ",__FILE__,__LINE__);
    ret = notpayin_to_table_lz(&notpayin);
    if (ret!=0)
    {
        vtcp_log("[%s][%d]���� notpayin_to_table_lz ִ�д���",__FILE__,__LINE__);
        sprintf( acErrMsg, "���� notpayin_to_table_lz ִ�д���!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "EP018" );
      return -1;
    }
    return 0;
  }
  

  vtcp_log("%s,%d cashqsactno=[%.12s]",__FILE__,__LINE__,wp_notpayin->cashqsactno);
  vtcp_log("[%s][%d] wp_notpayin->content==[%.60s] ",__FILE__,__LINE__,wp_notpayin->content);
  memset(cTempstr,0,sizeof(cTempstr));
  memcpy(cTempstr, wp_notpayin->content, sizeof(wp_notpayin->content));
  dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
  memcpy(wp_notpayin->content, cTempstr, sizeof(wp_notpayin->content));
  vtcp_log("[%s][%d] wp_notpayin->content==[%.60s] ",__FILE__,__LINE__,wp_notpayin->content);  

  vtcp_log("[%s][%d] wp_notpayin->name==[%.60s] ",__FILE__,__LINE__,wp_notpayin->name);
  memset(cTempstr,0,sizeof(cTempstr));
  memcpy(cTempstr, wp_notpayin->name, sizeof(wp_notpayin->name));
  dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
  memcpy(wp_notpayin->name, cTempstr, sizeof(wp_notpayin->name));
  vtcp_log("[%s][%d] wp_notpayin->name==[%.60s] ",__FILE__,__LINE__,wp_notpayin->name);  
  
  /*��cmt����*/
  vtcp_log("[%s][%d]=========iCmtno=[%d]\n",__FILE__,__LINE__,iCmtno);
  vtcp_log("[%s][%d]=========iPkgno=[%d]\n",__FILE__,__LINE__,iPkgno);
  switch(iCmtno)
  {
  case 319:
  case 320:
  case 321:
  case 322:
  case 323:
  case 324:
  case 325:
  case 327:
  case 328:
    /*���볷���˻س���ֹ�����͵Ǽǲ�*/
    {       
      struct hv_fzf_c    sHvFzf_s;
      struct hv_fzf_c    hv_fzf_c_s;
      struct hv_fzf_c    sHvFzf_r;
      struct hv_fzf_c    hv_fzf_c_r;
      struct lv_wbctl_c  sLvWbctl;
      struct lv_wbctl_c  lv_wbctl_c;

      memset(&sHvFzf_s, '\0', sizeof(struct hv_fzf_c));
      memset(&sHvFzf_r, '\0', sizeof(struct hv_fzf_c));
      memset(&sHvFzf_s, '\0', sizeof(struct hv_fzf_c));
      memset(&sHvFzf_r, '\0', sizeof(struct hv_fzf_c));
      memset(&sLvWbctl, '\0', sizeof(struct lv_wbctl_c));
      memset(&lv_wbctl_c, '\0', sizeof(struct lv_wbctl_c));
      hv_fzf_c_s.otype[0] = '2';
      memcpy(hv_fzf_c_s.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c_s.cmtno)-1);
      memcpy(hv_fzf_c_s.br_no, cBrno, BRNO_LEN);
      vtcp_log("[%s][%d]stat===[%c] \n",__FILE__,__LINE__,wp_notpayin->stat);
      /*С��ϵͳ����*/
      hv_fzf_c_s.tx_date = apatoi(pcLvGetDay(),8);
      hv_fzf_c_s.wt_date = apatoi(pcLvGetDay(),8);
      memcpy(hv_fzf_c_s.pay_qs_no,wp_notpayin->payqsactno,sizeof(hv_fzf_c_s.pay_qs_no)-1);
      memcpy(hv_fzf_c_s.req_br_no,wp_notpayin->orbrno,sizeof(hv_fzf_c_s.req_br_no)-1);
      vtcp_log("[%s][%d] reqbrno===[%s]\n",__FILE__,__LINE__,hv_fzf_c_s.req_br_no);
      memcpy(hv_fzf_c_s.orderno,wp_notpayin->orderno,sizeof(hv_fzf_c_s.orderno)-1);
      memcpy(hv_fzf_c_s.cash_qs_no,wp_notpayin->cashqsactno,sizeof(hv_fzf_c_s.cash_qs_no)-1);
      memcpy(hv_fzf_c_s.res_br_no,wp_notpayin->acbrno,sizeof(hv_fzf_c_s.res_br_no)-1);
      hv_fzf_c_s.reqtype[0] = wp_notpayin->rettype;
      hv_fzf_c_s.hv_fzf_sts[0] = wp_notpayin->stat;
      hv_fzf_c_s.o_wt_date=apatoi(wp_notpayin->owtday,8);
      hv_fzf_c_s.res_sts[0] = wp_notpayin->respcode;

      memcpy(hv_fzf_c_s.o_cmtno,wp_notpayin->opkgno,sizeof(hv_fzf_c_s.o_cmtno)-1);
      vtcp_log("[%s][%d]opkgno=[%.3s],[%.3s]\n",__FILE__,__LINE__,hv_fzf_c_s.o_cmtno,wp_notpayin->opkgno);
      memcpy(hv_fzf_c_s.o_packid,wp_notpayin->opackid,sizeof(hv_fzf_c_s.o_packid)-1);
      memcpy(hv_fzf_c_s.otxnum,wp_notpayin->otxnum,sizeof(hv_fzf_c_s.otxnum)-1);

      vtcp_log("[%s][%d]wp_notpayin->totamt====[%s]\n",__FILE__,__LINE__,wp_notpayin->totamt);   
      vtcp_log("[%s][%d]wp_notpayin->totcnt====[%s]\n",__FILE__,__LINE__,wp_notpayin->totcnt);
      vtcp_log("[%s][%d]iCmtno====[%d]\n",__FILE__,__LINE__,iCmtno);
      /*******************add by xyy 20060326********************/
      if (iCmtno==319 || iCmtno==327)
      {
        vtcp_log("[%s][%d]wp_notpayin->totamt====[%s]\n",__FILE__,__LINE__,wp_notpayin->totamt);   
        vtcp_log("[%s][%d]wp_notpayin->totcnt====[%s]\n",__FILE__,__LINE__,wp_notpayin->totcnt);
        vtcp_log("[%s][%d]iCmtno====[%d]\n",__FILE__,__LINE__,iCmtno);
        if (strlen(wp_notpayin->totamt)==0)
        {       
          ret = Lv_wbctl_Sel(g_pub_tx.reply, &sLvWbctl," pack_date=%d and packid = '%s'",
            apatoi(wp_notpayin->opackday,8),wp_notpayin->opackid);
          if ( ret == 1403 )
          {
            sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
            WRITEMSG
              strcpy( g_pub_tx.reply, "EP025" );
            return -1;
          }else if( ret )
          {
            sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
            WRITEMSG
              strcpy( g_pub_tx.reply, "EP065" );
            return -1;
          }
          if (wp_notpayin->rettype=='1')   /************�˻�������ʱ��**************/
          {   
            dbl2str(&lv_wbctl_c.dtlcnt,0,sizeof(wp_notpayin->totcnt),0,hv_fzf_c_s.filler); 
            dbl2str(&lv_wbctl_c.totamt,0,sizeof(wp_notpayin->totamt),0,hv_fzf_c_s.filler+sizeof(wp_notpayin->totcnt));
            vtcp_log("[%s][%d]lv_wbctl_c.dtlcnt=[%f]\n",__FILE__,__LINE__,lv_wbctl_c.dtlcnt);
            vtcp_log("[%s][%d]lv_wbctl_c.totamt=[%f]\n",__FILE__,__LINE__,lv_wbctl_c.totamt);
            vtcp_log("[%s][%d]whv_fzf_c_s.filler====[%s]\n",__FILE__,__LINE__,hv_fzf_c_s.filler);
          }
          else       /*********�˻ص��ʵ�ʱ��*********/
          {

            ret=0;
            memset(&wd_payin, 0 , sizeof(wd_payin));

            memcpy(wd_payin.F_opcd,OPCD_LR_QUERY,sizeof(wd_payin.F_opcd));
            memcpy(wd_payin.F_orbrno,wp_notpayin->orbrno,sizeof(wd_payin.F_orbrno));
            memcpy(wd_payin.F_wtday,wp_notpayin->owtday,sizeof(wd_payin.F_wtday));
            memcpy(wd_payin.F_orderno,wp_notpayin->oorderno,sizeof(wd_payin.F_orderno));
            memcpy(wd_payin.F_pkgno,wp_notpayin->opkgno,sizeof(wd_payin.F_pkgno));
            vtcp_log("[%s][%d],wp_notpayin->pkgno=[%.3s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
            vtcp_log("[%s][%d],wp_notpayin->orbrno=[%.12s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
            vtcp_log("[%s][%d],wp_notpayin->owtday=[%.8s]\n",__FILE__,__LINE__,wp_notpayin->owtday);
            vtcp_log("[%s][%d],wp_notpayin->orderno=[%.8s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);
            ret=iLvFetchRecToPayin(&wd_payin);
            if (ret)
            {
              vtcp_log("[%s][%d] ��ѯ֧�����״���[%d]\n",__FILE__,__LINE__,ret);
              sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
              WRITEMSG
                strcpy( g_pub_tx.reply, "EP017" );
              return ret;
            }
            memcpy(wp_notpayin->totcnt,"00000001",sizeof(wp_notpayin->totcnt));
            memcpy(wp_notpayin->totamt,wd_payin.T_txamt,sizeof(wp_notpayin->totamt));
            memcpy(hv_fzf_c_s.filler,wp_notpayin->totcnt,sizeof(wp_notpayin->totcnt));
            memcpy(hv_fzf_c_s.filler+sizeof(wp_notpayin->totcnt),wp_notpayin->totamt,sizeof(wp_notpayin->totamt));
            vtcp_log("[%s][%d]lv_wbctl_c.dtlcnt=[%f]\n",__FILE__,__LINE__,lv_wbctl_c.dtlcnt);
            vtcp_log("[%s][%d]lv_wbctl_c.totamt=[%f]\n",__FILE__,__LINE__,lv_wbctl_c.totamt);
          }  
        }
        else
        {
          memcpy(hv_fzf_c_s.filler,wp_notpayin->totcnt,sizeof(wp_notpayin->totcnt));
          memcpy(hv_fzf_c_s.filler+sizeof(wp_notpayin->totcnt),wp_notpayin->totamt,sizeof(wp_notpayin->totamt));
        }
        vtcp_log("[%s][%d]wp_notpay->packid====[%s]\n",__FILE__,__LINE__,wp_notpayin->opackid);
        vtcp_log("[%s][%d]whv_fzf_c_s.filler====[%s]\n",__FILE__,__LINE__,hv_fzf_c_s.filler);
      }
      else
      {
        memcpy(hv_fzf_c_s.filler,wp_notpayin->totcnt,sizeof(wp_notpayin->totcnt));
        memcpy(hv_fzf_c_s.filler+sizeof(wp_notpayin->totcnt),wp_notpayin->totamt,sizeof(wp_notpayin->totamt));
      }
      vtcp_log("[%s][%d]*******hv_fzf_c_s.filler=[%s]\n",__FILE__,__LINE__,hv_fzf_c_s.filler);
      if (iCmtno == 320)
      {
        memcpy(cBakcode, wp_notpayin->acbrno, sizeof(cBakcode)-1);
        memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
        ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf_r," otype = '1' and or_br_no = '%s' and wt_date=%d and orderno = '%s' and cmtno = '319'", cBakcode,apatoi(wp_notpayin->owtday,8), cOrderno);
        if ( ret == 1403 )
        {
          sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
          WRITEMSG
          strcpy( g_pub_tx.reply, "EP020" );
          return -1;
        }else if( ret )
        {
          sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
          WRITEMSG
          return -1;
        }
        hv_fzf_c_s.o_pack_date=sHvFzf_r.wt_date;
        hv_fzf_c_s.o_wt_date=sHvFzf_r.wt_date;
        hv_fzf_c_s.reqtype[0]=sHvFzf_r.reqtype[0];
      }
      else if (iCmtno == 328)
      {
        memcpy(cBakcode, wp_notpayin->acbrno, sizeof(cBakcode)-1);
        memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
        ret = Hv_fzf_Sel(g_pub_tx.reply, &sHvFzf_r," otype = '1' and or_br_no = '%s' and wt_date = '%d' and orderno = '%s' and cmtno = '327'", cBakcode,apatoi(wp_notpayin->owtday,8), cOrderno);
        if ( ret == 1403 )
        {
          sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
          WRITEMSG
          strcpy( g_pub_tx.reply, "EP020" );
          return -1;
        }else if( ret )
        {
          sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
          WRITEMSG
          return -1;
        }
        hv_fzf_c_s.o_pack_date=apatoi(sHvFzf_r.wt_date,8);
        hv_fzf_c_s.o_wt_date=apatoi(sHvFzf_r.wt_date,8);
        memcpy(hv_fzf_c_s.reqtype,sHvFzf_r.reqtype,sizeof(hv_fzf_c_s.reqtype)-1);
      }
      else
      {
        hv_fzf_c_s.o_pack_date=apatoi(sHvFzf_r.wt_date,8);
        hv_fzf_c_s.o_wt_date=apatoi(sHvFzf_r.wt_date,8);
      }
      vtcp_log("[%s][%d]wd_notpayin->respcode=[%c]\n", __FILE__,__LINE__,wp_notpayin->respcode);
      memcpy(hv_fzf_c_s.o_orderno,wp_notpayin->oorderno,sizeof(hv_fzf_c_s.o_orderno)-1);

      if (iCmtno==319 || iCmtno==321 || iCmtno==323 || iCmtno==327)
        memcpy(hv_fzf_c_s.req_content,wp_notpayin->content,sizeof(hv_fzf_c_s.req_content)-1);
      if (iCmtno==320 || iCmtno==328)
        memcpy(hv_fzf_c_s.res_content,wp_notpayin->content,sizeof(hv_fzf_c_s.res_content)-1);

      hv_fzf_c_s.otype[0] = '1';

      vtcp_log("[%s][%d]====��ӡ�����ݿ�ʼ!!\n", __FILE__,__LINE__);
      ret=Hv_fzf_Debug(&hv_fzf_c_s);
      vtcp_log("[%s][%d]====��ӡ�����ݽ���!![%d]\n", __FILE__,__LINE__,ret);
      
    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    if(strlen(hv_fzf_c_s.sendco) < 4)
    {
        iGetSendco(hv_fzf_c_s.sendco, hv_fzf_c_s.pay_qs_no);
    }
    if(strlen(hv_fzf_c_s.receco) < 4)
    {
        iGetSendco(hv_fzf_c_s.receco, hv_fzf_c_s.cash_qs_no);
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
      ret = Hv_fzf_Ins(hv_fzf_c_s, g_pub_tx.reply);
      if ( ret )
      {
        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "EP010" );
        return -1;
      }
      
      struct hv_fzfhtr_c hv_fzfhtr;
      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
      
      iLvFzfToFzfhtr(&hv_fzf_c_s,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/

      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
      if(ret)
      {
              sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
              WRITEMSG
              strcpy(g_pub_tx.reply,"HV01");
              return (-1);
      }
      
      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);      
    }
    break;
  case 301:
    /*�����ѯ���ķ��͵Ǽǲ�*/
    {
      struct hv_fzf_c    sHvFzf_s;
      struct hv_fzf_c    hv_fzf_c;
      memset(&sHvFzf_s, '\0', sizeof(struct hv_fzf_c));
      memset(&hv_fzf_c, '\0', sizeof(struct hv_fzf_c));
      memcpy(hv_fzf_c.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c.cmtno)-1);      
      hv_fzf_c.otype[0] = '2';
      
      hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
       hv_fzf_c.wt_date = apatoi(pcLvGetDay(),8);
                            
      memcpy(hv_fzf_c.sendco, wp_notpayin->sendco, sizeof(hv_fzf_c.sendco)-1);
      memcpy(hv_fzf_c.pay_qs_no,wp_notpayin->payqsactno,sizeof(hv_fzf_c.pay_qs_no)-1);
      memcpy(hv_fzf_c.or_br_no, wp_notpayin->orbrno, sizeof(hv_fzf_c.or_br_no)-1);
      memcpy(hv_fzf_c.orderno, wp_notpayin->orderno, sizeof(hv_fzf_c.orderno)-1);
      memcpy(hv_fzf_c.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
      memcpy(hv_fzf_c.ac_br_no, wp_notpayin->acbrno, sizeof(hv_fzf_c.ac_br_no)-1);

      hv_fzf_c.o_wt_date=apatoi(wp_notpayin->owtday,8);
      memcpy(hv_fzf_c.o_or_br_no,wp_notpayin->oorbrno,sizeof(hv_fzf_c.o_or_br_no)-1);
      memcpy(hv_fzf_c.o_ac_br_no,wp_notpayin->oacbrno,sizeof(hv_fzf_c.o_ac_br_no)-1);
      hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
      memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
      str2dbl(wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt),0,&hv_fzf_c.o_tx_amt);
      memcpy(hv_fzf_c.req_content,wp_notpayin->content,sizeof(hv_fzf_c.req_content)-1);
      hv_fzf_c.tx_time = apatoi(cTime,6);
      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
      hv_fzf_c.send_trace_no=apatoi(wp_notpayin->wssrno,sizeof(wp_notpayin->wssrno));
      Hv_fzf_Debug(&hv_fzf_c);
      
    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    if(strlen(hv_fzf_c.sendco) < 4)
    {
        iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);

    }
    if(strlen(hv_fzf_c.receco) < 4)
    {
        iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/       
      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
      if ( ret )
      {
        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
        strcpy( g_pub_tx.reply, "EP010" );
        WRITEMSG
          return -1;
      }
      
      struct hv_fzfhtr_c hv_fzfhtr;
      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
      
      iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/

      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
      if(ret)
      {
              sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
              WRITEMSG
              strcpy(g_pub_tx.reply,"HV01");
              return (-1);
      }
      
      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);        
      
    }
    break;
  case 302:
    /*����鸴���ķ��͵Ǽǲ�*/
    {
      struct hv_fzf_c    sHvFzf_s;
      struct hv_fzf_c    hv_fzf_c_s;
      struct hv_fzf_c    sHvFzf_r;
      struct hv_fzf_c    hv_fzf_c_r;

      memset(&sHvFzf_s, '\0', sizeof(struct hv_fzf_c));
      memset(&sHvFzf_r, '\0', sizeof(struct hv_fzf_c));
      memset(&hv_fzf_c_s, '\0', sizeof(struct hv_fzf_c));
      memset(&hv_fzf_c_r, '\0', sizeof(struct hv_fzf_c));
      hv_fzf_c_s.otype[0] = '2';
      memcpy(hv_fzf_c_s.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c_s.cmtno)-1);
      hv_fzf_c_s.tx_date = apatoi(pcLvGetDay(),8);
      hv_fzf_c_s.wt_date = apatoi(pcLvGetDay(),8);
      memcpy(hv_fzf_c_s.sendco, wp_notpayin->sendco, sizeof(hv_fzf_c_s.sendco)-1);
      memcpy(hv_fzf_c_s.pay_qs_no, wp_notpayin->payqsactno, sizeof(hv_fzf_c_s.pay_qs_no)-1);
      memcpy(hv_fzf_c_s.res_br_no, wp_notpayin->orbrno, sizeof(hv_fzf_c_s.res_br_no)-1);
      memcpy(hv_fzf_c_s.orderno, wp_notpayin->orderno, sizeof(hv_fzf_c_s.orderno)-1);
      memcpy(hv_fzf_c_s.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
      memcpy(hv_fzf_c_s.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c_s.cash_qs_no)-1);
      memcpy(hv_fzf_c_s.req_br_no, wp_notpayin->acbrno, sizeof(hv_fzf_c_s.req_br_no)-1);
      hv_fzf_c_s.wt_date=apatoi(wp_notpayin->qwtday,8);

      hv_fzf_c_r.req_date = apatoi(wp_notpayin->qwtday, 8);
      memcpy(hv_fzf_c_r.or_br_no, wp_notpayin->acbrno, sizeof(hv_fzf_c_r.or_br_no)-1);
      memcpy(hv_fzf_c_r.orderno, wp_notpayin->qorderno, sizeof(hv_fzf_c_r.orderno)-1);
      ret = Hv_fzf_Sel(g_pub_tx.reply, &hv_fzf_c_r, " otype = '1' and or_br_no = '%s' and wt_date = '%d' and orderno = '%s' and cmtno = '301'",wp_notpayin->acbrno,apatoi(wp_notpayin->qwtday,8),wp_notpayin->qorderno);
      if ( ret == 1403 )
      {
        sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "EP010" );
        return -1;
      }else if( ret )
      {
        sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
        WRITEMSG
        return -1;
      }
      hv_fzf_c_s.o_wt_date =  hv_fzf_c_r.o_wt_date;

      memcpy(hv_fzf_c_s.o_or_br_no,hv_fzf_c_r.o_or_br_no,sizeof(hv_fzf_c_s.o_or_br_no)-1);
      memcpy(hv_fzf_c_s.o_ac_br_no,hv_fzf_c_r.o_ac_br_no,sizeof(hv_fzf_c_s.o_ac_br_no)-1);
      hv_fzf_c_s.otype[0] = wp_notpayin->otxtype;
      hv_fzf_c_s.o_tx_type[0] = wp_notpayin->stat;
      /*ԭ��ѯ���*/
      memcpy(hv_fzf_c_s.req_orderno, hv_fzf_c_r.orderno, sizeof(hv_fzf_c_s.req_orderno)-1);
      /*ԭ֧���������*/
      memcpy(hv_fzf_c_s.o_orderno, hv_fzf_c_r.o_orderno, sizeof(hv_fzf_c_s.o_orderno)-1);
      hv_fzf_c_s.o_tx_amt=hv_fzf_c_r.o_tx_amt;
      memcpy(hv_fzf_c_s.res_content,wp_notpayin->content,sizeof(hv_fzf_c_s.res_content)-1);
      hv_fzf_c_s.tx_time = apatoi(cTime,6);
      memcpy(hv_fzf_c_s.br_no,cBrno,sizeof(hv_fzf_c_s.br_no)-1);
      memcpy(hv_fzf_c_s.tel,cTlrno,sizeof(hv_fzf_c_s.tel)-1);
      hv_fzf_c_s.send_trace_no=apatoi(wp_notpayin->wssrno,sizeof(wp_notpayin->wssrno));
      
    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    if(strlen(hv_fzf_c_s.sendco) < 4)
    {
        iGetSendco(hv_fzf_c_s.sendco, hv_fzf_c_s.pay_qs_no);
    }
    if(strlen(hv_fzf_c_s.receco) < 4)
    {
        iGetSendco(hv_fzf_c_s.receco, hv_fzf_c_s.cash_qs_no);
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/ 
      
      ret = Hv_fzf_Ins(hv_fzf_c_s, g_pub_tx.reply);
      if ( ret )
      {
        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "EP010" );
        return -1;
      }
        
      struct hv_fzfhtr_c hv_fzfhtr;
      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
      
      iLvFzfToFzfhtr(&hv_fzf_c_s,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/

      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
      if(ret)
      {
              sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
              WRITEMSG
              strcpy(g_pub_tx.reply,"HV01");
              return (-1);
      }
      
      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);      
    }
    break;
  case 303:
    /*�������ɸ�ʽ���ķ��͵Ǽǲ�*/
    {
      struct hv_fzf_c    sHvFzf_s;
      struct hv_fzf_c    hv_fzf_c;
      memset(&sHvFzf_s, '\0', sizeof(struct hv_fzf_c));
      memset(&hv_fzf_c, '\0', sizeof(struct hv_fzf_c));
      hv_fzf_c.otype[0] = '2';
      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
      memcpy(hv_fzf_c.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c.cmtno)-1);
      hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
      hv_fzf_c.wt_date = apatoi(pcLvGetDay(),8);
      memcpy(hv_fzf_c.sendco, wp_notpayin->sendco, sizeof(hv_fzf_c.sendco)-1);
      memcpy(hv_fzf_c.pay_qs_no, wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
      memcpy(hv_fzf_c.or_br_no, wp_notpayin->orbrno, sizeof(hv_fzf_c.or_br_no)-1);
      memcpy(hv_fzf_c.orderno, wp_notpayin->orderno, sizeof(hv_fzf_c.orderno)-1);
      memcpy(hv_fzf_c.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
      vtcp_log("%s,%d cashqsactno=[%.12s]",__FILE__,__LINE__,wp_notpayin->cashqsactno);
      memcpy(hv_fzf_c.ac_br_no, wp_notpayin->acbrno, sizeof(hv_fzf_c.ac_br_no)-1);
      memcpy(hv_fzf_c.req_content,wp_notpayin->content,sizeof(hv_fzf_c.req_content)-1);
      hv_fzf_c.tx_time = apatoi(cTime,6);
      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
      hv_fzf_c.send_trace_no=apatoi(wp_notpayin->wssrno,sizeof(wp_notpayin->wssrno));

    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    if(strlen(hv_fzf_c.sendco) < 4)
    {
        iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
    }
    if(strlen(hv_fzf_c.receco) < 4)
    {
        iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/ 
      
      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
      if ( ret )
      {
        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "EP010" );
        return -1;
      }
      struct hv_fzfhtr_c hv_fzfhtr;
      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
      
      iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/

      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
      if(ret)
      {
              sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
              WRITEMSG
              strcpy(g_pub_tx.reply,"HV01");
              return (-1);
      }
      
      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);        
      
    }
    break;
  }
  /*��pkg����*/
  switch(iPkgno)
  {
    case 12:
    /*����ͨ����Ϣ���͵Ǽǲ�*/
    {
      memcpy(sLvPkgreg.pkgno, wp_notpayin->pkgno, sizeof(sLvPkgreg.pkgno)-1);
      sLvPkgreg.in_date=apatoi( pcLvGetDay(),8);
      sLvPkgreg.wt_date=sLvPkgreg.in_date;
      sLvPkgreg.pack_date=sLvPkgreg.in_date;
      memcpy(sLvPkgreg.orderno, wp_notpayin->orderno, sizeof(sLvPkgreg.orderno)-1);
      get_zd_long(DC_TIME,&sLvPkgreg.tx_time);
      memcpy(sLvPkgreg.txnum, wp_notpayin->txnum, sizeof(sLvPkgreg.txnum)-1);
      memcpy(sLvPkgreg.or_br_no, wp_notpayin->orbrno, sizeof(sLvPkgreg.or_br_no)-1);
      memcpy(sLvPkgreg.pay_qs_no, wp_notpayin->payqsactno, sizeof(sLvPkgreg.pay_qs_no)-1);
      memcpy(sLvPkgreg.sendco, wp_notpayin->sendco, sizeof(sLvPkgreg.sendco)-1);
      memcpy(sLvPkgreg.ac_br_no, wp_notpayin->acbrno, sizeof(sLvPkgreg.ac_br_no)-1);
      memcpy(sLvPkgreg.cash_qs_no, wp_notpayin->cashqsactno, sizeof(sLvPkgreg.cash_qs_no)-1);
      memcpy(sLvPkgreg.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
      /*������������*/
      sLvPkgreg.lv_o_sts[0] = wp_notpayin->datatype;
      /** wp_notpayin->content��255���ȣ�����sLvPkgreg.lv_brfֻ��60��?? **/
      memcpy(sLvPkgreg.lv_brf, wp_notpayin->content, sizeof(sLvPkgreg.lv_brf)-1);
      if (!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN) || !memcmp(wp_notpayin->txnum,TXNUM_DF,TXNUM_LEN))
      {
/********** ��������ôд

        if (!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN))
          sLvPkgreg.sfflag[0] = '0';
        else
          sLvPkgreg.sfflag[0] = '1';
        memcpy(sLvPkgreg.sfunit, wp_notpayin->unit, sizeof(sLvPkgreg.sfunit)-1);
        memcpy(sLvPkgreg.sftype, wp_notpayin->sftype, sizeof(sLvPkgreg.sftype)-1);
        memcpy(sLvPkgreg.addid, wp_notpayin->addid, sizeof(sLvPkgreg.addid)-1);
**********/
      }
      get_zd_data(DC_TX_BR_NO  ,sLvPkgreg.br_no     );
      get_zd_data(DC_TEL       ,sLvPkgreg.input_tel   );
      get_zd_data(DC_TRACE_NO,sLvPkgreg.input_trace_no);
      memcpy(sLvPkgreg.packid,wp_notpayin->opackid,sizeof(sLvPkgreg.packid)-1);

      /*���Ӽ���ʱ��״̬*/
      if (memcmp(wp_notpayin->optype, OPCD_LR, 2) == 0)
        sLvPkgreg.lv_sts[0] = STAT_WZLR;
      else if (memcmp(wp_notpayin->optype, OPCD_SEND, 2) == 0)
        sLvPkgreg.lv_sts[0] = STAT_WZSEND;
      /*�������ݳ��Ⱥ���������*/
/***
      memcpy(sLvPkgreg.cyc, wp_notpayin->belongcyc, sizeof(sLvPkgreg.cyc)-1);
      if (!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN) || !memcmp(wp_notpayin->txnum,TXNUM_DF,TXNUM_LEN))
        memcpy(sLvPkgreg.infolen, "00000078", sizeof(sLvPkgreg.infolen)-1);
***/
      /*�������ݳ��Ⱥ���������*/
    vtcp_log("[%s][%d]--------��ʼ�Ǳ��ˣ�����",__FILE__,__LINE__);
      Lv_pkgreg_Debug(&sLvPkgreg);
      
    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    if(strlen(sLvPkgreg.sendco) < 4)
    {
        iGetSendco(sLvPkgreg.sendco, sLvPkgreg.pay_qs_no);
    }
    if(strlen(sLvPkgreg.receco) < 4)
    {
        iGetSendco(sLvPkgreg.receco, sLvPkgreg.cash_qs_no);
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
      
      ret=Lv_pkgreg_Ins( sLvPkgreg , g_pub_tx.reply );
      if (ret != DB_OK)
      {
        vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,ret);
        Lv_pkgreg_Debug(&sLvPkgreg);
        sprintf(acErrMsg,"������Ϣ���͵Ǽǲ�����!",40);
        return ret;
      }
    }
    break;
  case 13:
    /*����ʵʱ��Ϣҵ���͵Ǽǲ�*/
    {
      sLvPkgreg.in_date=apatoi( pcLvGetDay(),8);
      sLvPkgreg.wt_date=sLvPkgreg.in_date;
      sLvPkgreg.pack_date=sLvPkgreg.in_date;
      memcpy(sLvPkgreg.orderno, wp_notpayin->orderno,  sizeof(sLvPkgreg.orderno)-1);
      get_zd_long(DC_TIME,&sLvPkgreg.tx_time);
      memcpy(sLvPkgreg.txnum,   wp_notpayin->txnum,   sizeof(sLvPkgreg.txnum)-1);
      memcpy(sLvPkgreg.or_br_no,   wp_notpayin->orbrno,   sizeof(sLvPkgreg.or_br_no)-1);
      memcpy(sLvPkgreg.pay_qs_no, wp_notpayin->payqsactno, sizeof(sLvPkgreg.pay_qs_no)-1);
      memcpy(sLvPkgreg.sendco,   wp_notpayin->sendco,   sizeof(sLvPkgreg.sendco)-1);
      memcpy(sLvPkgreg.ac_br_no,   wp_notpayin->acbrno,   sizeof(sLvPkgreg.ac_br_no)-1);
      memcpy(sLvPkgreg.cash_qs_no, wp_notpayin->cashqsactno, sizeof(sLvPkgreg.cash_qs_no)-1);
      memcpy(sLvPkgreg.receco,   wp_notpayin->receco,   sizeof(wp_notpayin->receco));
      memcpy(sLvPkgreg.pkgno,   wp_notpayin->pkgno,   sizeof(sLvPkgreg.pkgno)-1);
      sLvPkgreg.lv_o_sts[0] = wp_notpayin->datatype;
      sLvPkgreg.lv_sts[0] = wp_notpayin->stat;
      sLvPkgreg.lw_ind[0] = '1';/**NEWTCTD:���ߵ��������ʵ������**/
      memcpy(sLvPkgreg.lv_brf, wp_notpayin->content, sizeof(sLvPkgreg.lv_brf)-1);
      if (!memcmp(wp_notpayin->txnum,TXNUM_ZHCX,TXNUM_LEN) || !memcmp(wp_notpayin->txnum,TXNUM_CXYD,TXNUM_LEN))
			{
/***********
        sLvPkgreg.qacttype[0] = wp_notpayin->acttype;
        memcpy(sLvPkgreg.qactno,   wp_notpayin->actno,   sizeof(sLvPkgreg.qactno)-1);
        memcpy(sLvPkgreg.qname,   wp_notpayin->name,   sizeof(sLvPkgreg.qname)-1);
        sLvPkgreg.qpswdcd[0] = wp_notpayin->pswdcd;
        memcpy(sLvPkgreg.qpasswd, wp_notpayin->pascode,   sizeof(sLvPkgreg.qpasswd)-1);
        sLvPkgreg.qtype[0] = wp_notpayin->qtype;
        memcpy(sLvPkgreg.qcontent, wp_notpayin->reason,   sizeof(sLvPkgreg.qcontent)-1);
        str2dbl(wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt),0,&sLvPkgreg.qavbal);
****************/
			}
      memcpy(sLvPkgreg.packid,wp_notpayin->opackid,sizeof(sLvPkgreg.packid)-1);
      sLvPkgreg.o_pack_date=apatoi(wp_notpayin->opackday,8);
      memcpy(sLvPkgreg.o_packid,   wp_notpayin->opackid, sizeof(sLvPkgreg.o_packid)-1);
      memcpy(sLvPkgreg.addid,   wp_notpayin->addid,   sizeof(sLvPkgreg.addid)-1);
      get_zd_data(DC_TX_BR_NO  ,sLvPkgreg.br_no     );
      get_zd_data(DC_TEL       ,sLvPkgreg.input_tel   );
      get_zd_data(DC_TRACE_NO,sLvPkgreg.input_trace_no);
      
    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    if(strlen(sLvPkgreg.sendco) < 4)
    {
        iGetSendco(sLvPkgreg.sendco, sLvPkgreg.pay_qs_no);
    }
    if(strlen(sLvPkgreg.receco) < 4)
    {
        iGetSendco(sLvPkgreg.receco, sLvPkgreg.cash_qs_no);
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
     	/**TODO****/
			Lv_pkgreg_Debug(&sLvPkgreg); 
      ret=Lv_pkgreg_Ins( sLvPkgreg , g_pub_tx.reply );
      if (ret != DB_OK)
      {
        vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,ret);
        Lv_pkgreg_Debug(&sLvPkgreg);
        sprintf(acErrMsg,"������Ϣ���͵Ǽǲ�����!",40);
        return ret;
      }
		}
    break;
  }
  return 0;
}



/*ע�����ʱ����״̬*/
int notpayin_to_table_lz(NOTPAY_IN_AREA *wp_notpayin)
{
  char  cCmtno[4]; 
  char  cPkgno[4];
  int   iPkgno=0;
  int    iCmtno=0;
  int   ret=0;
  char  cTempstr[1024];
  char  cBrno[6];
  char  cTime[7];
  char  cTlrno[5];
  char  cTraceno[7];  
  struct hv_uniontab_c sHv_uniontab;
          
  memset(cCmtno, '\0', sizeof(cCmtno));
  memset(cPkgno, '\0', sizeof(cPkgno));

  memset(cBrno, 0, sizeof(cBrno));
  memset(cTime, 0, sizeof(cTime));
  memset(cTlrno, 0, sizeof(cTlrno));
  memset(cTraceno, 0, sizeof(cTraceno));
  
  get_fd_data("0030",cBrno);/* ���׻��� */
  get_fd_data("0060",cTime);/* ����ʱ�� */
  get_fd_data("0070",cTlrno);  /* ����Ա */
  get_fd_data("0040",cTraceno);  /* ��ˮ�� */

  vtcp_log("[%s][%d]--------------wp_notpayin->cmtno=[%s]\n",__FILE__,__LINE__,wp_notpayin->cmtno);
  vtcp_log("%s,%d cashqsactno====[%.12s]",__FILE__,__LINE__,wp_notpayin->cashqsactno);
  memcpy(cCmtno, wp_notpayin->cmtno, sizeof(cCmtno)-1);
  memcpy(cPkgno, wp_notpayin->pkgno, sizeof(cPkgno)-1);

  iCmtno = atoi(cCmtno);
  iPkgno = atoi(cPkgno);

  vtcp_log("[%s][%d]---------iCmtno=[%d]  iPkgno=[%d]\n",__FILE__,__LINE__,iCmtno,iPkgno);

  memset(cTempstr,0,sizeof(cTempstr));
  memcpy(cTempstr, wp_notpayin->content, sizeof(wp_notpayin->content));
  dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
  memcpy(wp_notpayin->content, cTempstr, sizeof(wp_notpayin->content));
  vtcp_log("[%s][%d] wp_notpayin->content==[%.60s] ",__FILE__,__LINE__,wp_notpayin->content);  
  vtcp_log("%s,%d cashqsactno====[%.12s]",__FILE__,__LINE__,wp_notpayin->cashqsactno);
  
  if(iCmtno > 0)   /*��cmt����*/
  {
       vtcp_log("[%s][%d]+++++++++++++ִ�е���������!+++iCmtno=[%d]\n",__FILE__,__LINE__,iCmtno);    
      switch(iCmtno)
      {
           case 301:   /*�����ѯ���ķ��͵Ǽǲ�*/
              {
                  struct hv_fzf_c    hv_fzf_c;
                  memset(&hv_fzf_c, '\0', sizeof(struct hv_fzf_c));
                  memcpy(hv_fzf_c.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c.cmtno)-1);

                  hv_fzf_c.otype[0] = '2';
                  hv_fzf_c.lw_ind[0] = '2';   /*������ʶ*/
                  hv_fzf_c.hv_prt_ind[0] = '0';    

                  /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
                  Modified by ChenMing  2006.10.10.                  **/  
                  hv_fzf_c.wt_date = apatoi(wp_notpayin->txday,8);    
                  hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
                  vtcp_log("[%s][%d] hv_fzf_c.wt_date=[%d]    ",__FILE__,__LINE__,hv_fzf_c.wt_date);  
                  vtcp_log("[%s][%d] hv_fzf_c.tx_date=[%d] ",__FILE__,__LINE__,hv_fzf_c.tx_date);  

                  memcpy(hv_fzf_c.sendco, wp_notpayin->sendco, sizeof(hv_fzf_c.sendco)-1);
                  memcpy(hv_fzf_c.pay_qs_no,wp_notpayin->payqsactno,sizeof(hv_fzf_c.pay_qs_no)-1);
                  memcpy(hv_fzf_c.or_br_no, wp_notpayin->orbrno, sizeof(hv_fzf_c.or_br_no)-1);
                  memcpy(hv_fzf_c.orderno, wp_notpayin->orderno, sizeof(hv_fzf_c.orderno)-1);
                  memcpy(hv_fzf_c.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
                  memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
                  memcpy(hv_fzf_c.ac_br_no, wp_notpayin->acbrno, sizeof(hv_fzf_c.ac_br_no)-1);

                  hv_fzf_c.o_wt_date = apatoi(wp_notpayin->owtday,8);
                  hv_fzf_c.o_tx_date = hv_fzf_c.o_wt_date;

                  memcpy(hv_fzf_c.o_or_br_no,wp_notpayin->oorbrno,sizeof(hv_fzf_c.o_or_br_no)-1);
                  memcpy(hv_fzf_c.o_ac_br_no,wp_notpayin->oacbrno,sizeof(hv_fzf_c.o_ac_br_no)-1);
                  hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
                  hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
                  memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
                  str2dbl(wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt),0,&hv_fzf_c.o_tx_amt);
                  hv_fzf_c.o_tx_amt = hv_fzf_c.o_tx_amt/100.0; /*  ChenMing 2006-11-7 16:17*/
                  
                  hv_fzf_c.req_date = apatoi(wp_notpayin->txday,8);
                  memcpy(hv_fzf_c.req_br_no,wp_notpayin->orbrno,sizeof(hv_fzf_c.req_br_no)-1);
                  memcpy(hv_fzf_c.req_content,wp_notpayin->content,sizeof(hv_fzf_c.req_content)-1);
                  memcpy(hv_fzf_c.req_orderno,wp_notpayin->orderno,sizeof(hv_fzf_c.orderno)-1);
 
                  vtcp_log("[%s][%d] hv_fzf_c.req_orderno=[%s] ",__FILE__,__LINE__,hv_fzf_c.req_orderno);  

                  hv_fzf_c.tx_time = apatoi(cTime,8);
                  memcpy(hv_fzf_c.br_no, cBrno, sizeof(hv_fzf_c.br_no)-1);
                  memcpy(hv_fzf_c.tel, cTlrno, sizeof(hv_fzf_c.tel)-1);
                  hv_fzf_c.trace_no = apatoi(cTraceno,6);
                  memcpy(hv_fzf_c.o_cur_ind, wp_notpayin->cour, sizeof(hv_fzf_c.o_cur_ind)-1);  

                  hv_fzf_c.send_trace_no = apatoi(wp_notpayin->wssrno,sizeof(wp_notpayin->wssrno));
                  /* Hv_fzf_Debug(&hv_fzf_c); */
                  
                  /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
                  if(strlen(hv_fzf_c.sendco) < 4)
                  {
                      iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
                  }
                  if(strlen(hv_fzf_c.receco) < 4)
                  {
                      iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
                  }
                  /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
                                
                  ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
                  if ( ret )
                  {
                      sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
                      strcpy( g_pub_tx.reply, "EP010" );
                      WRITEMSG
                      return -1;
                  }
                  
                  struct hv_fzfhtr_c hv_fzfhtr;
                  memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
                  
                  iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);   /** ���µ� hv_fzf ->hv_fzfhtr�Ŀ���***/
            
                  ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
                  if(ret)
                  {
                          sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                          WRITEMSG
                          strcpy(g_pub_tx.reply,"HV01");
                          return (-1);
                  }
                  
                  vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);        
               
              }
              break;
           case 303:    /*�������ɸ�ʽ���ķ��͵Ǽǲ�*/
              {

                  struct hv_fzf_c    hv_fzf_c;
                  memset(&hv_fzf_c, '\0', sizeof(struct hv_fzf_c));

                  hv_fzf_c.otype[0] = '2';
                  hv_fzf_c.lw_ind[0] = '2';   /*������ʶ*/
                  hv_fzf_c.hv_prt_ind[0] = '0';    
 
                  /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
                  Modified by ChenMing  2006.10.10.                  **/  
                  hv_fzf_c.wt_date = apatoi(wp_notpayin->txday,8);    
                  hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
                  vtcp_log("[%s][%d] hv_fzf_c.wt_date=[%d]    ",__FILE__,__LINE__,hv_fzf_c.wt_date);  
                  vtcp_log("[%s][%d] hv_fzf_c.tx_date=[%d] ",__FILE__,__LINE__,hv_fzf_c.tx_date);  
 
 
                  hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
                  memcpy(hv_fzf_c.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c.cmtno)-1);

                  memcpy(hv_fzf_c.sendco, wp_notpayin->sendco, sizeof(hv_fzf_c.sendco)-1);
                  memcpy(hv_fzf_c.pay_qs_no, wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
                  memcpy(hv_fzf_c.or_br_no, wp_notpayin->orbrno, sizeof(hv_fzf_c.or_br_no)-1);
   
                  memcpy(hv_fzf_c.orderno, wp_notpayin->orderno, sizeof(hv_fzf_c.orderno)-1);
                  memcpy(hv_fzf_c.req_orderno, wp_notpayin->orderno, sizeof(hv_fzf_c.req_orderno)-1);
   
                  memcpy(hv_fzf_c.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
                  memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
                  vtcp_log("%s,%d cashqsactno====[%.12s]",__FILE__,__LINE__,wp_notpayin->cashqsactno);
                  memcpy(hv_fzf_c.ac_br_no, wp_notpayin->acbrno, sizeof(hv_fzf_c.ac_br_no)-1);
                  /* begin add by LiuHuafeng 20070501 */
                  if(strlen(hv_fzf_c.ac_br_no)==0)
                  {
                      memcpy(hv_fzf_c.ac_br_no,hv_fzf_c.cash_qs_no,sizeof(hv_fzf_c.ac_br_no)-1);
                  }
   
                  memcpy(hv_fzf_c.req_content,wp_notpayin->content,sizeof(hv_fzf_c.req_content)-1);
   
                  /*hv_fzf_c.o_wt_date = apatoi(wp_notpayin->owtday,8);
                    hv_fzf_c.o_tx_date = hv_fzf_c.o_wt_date;*/
                     
                   hv_fzf_c.tx_time = apatoi(cTime,8);
                   memcpy(hv_fzf_c.br_no, cBrno, sizeof(hv_fzf_c.br_no)-1);
                   memcpy(hv_fzf_c.tel, cTlrno, sizeof(hv_fzf_c.tel)-1);
                   hv_fzf_c.trace_no = apatoi(cTraceno,6);
                  /* memcpy(hv_fzf_c.o_cur_ind, wp_notpayin->cour, sizeof(hv_fzf_c.o_cur_ind)-1);   */
                  memcpy(hv_fzf_c.o_cur_ind, "RMB", sizeof(hv_fzf_c.o_cur_ind)-1);   /*����д����*/
                  hv_fzf_c.send_trace_no=apatoi(wp_notpayin->wssrno,sizeof(wp_notpayin->wssrno));

                  /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
                  if(strlen(hv_fzf_c.sendco) < 4)
                  {
                      iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
                  }
                  if(strlen(hv_fzf_c.receco) < 4)
                  {
                      iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
                  }
                  /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/

                  ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
                  if ( ret )
                  {
                      sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
                      WRITEMSG
                      strcpy( g_pub_tx.reply, "EP010" );
                      return -1;
                  }
                  struct hv_fzfhtr_c hv_fzfhtr;
                  memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
                  
                  iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/
            
                  ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
                  if(ret)
                  {
                       sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                       WRITEMSG
                       strcpy(g_pub_tx.reply,"HV01");
                       return (-1);
                  }
                  
                  vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);        
                  
              }
              break;              

      /* ����5�����Ķ���Ҫ�ҵ�ԭʼ���뽻�ף�Ȼ�����hv_zf���е����� */

           case 302:  /*����鸴���ķ��͵Ǽǲ�*/
              {
                  /* ��ԭ��ϵͳ��ͬ���鵽ԭ301��¼�����ԭ301��¼��û�ҵ��Ų���*/
                  char cOr_br_no[13];
                  char cWt_date[9];
                  char cOrderno[9];    
                  struct hv_fzf_c sHvFzf_Upd;            
                  struct hv_fzf_c hv_fzf_c;   
                  
                  /**add by xyy YHBP �����������**/   
                  memset(cOr_br_no , 0 , sizeof(cOr_br_no));
                  memset(cWt_date  , 0 , sizeof(cWt_date));
                  memset(cOrderno  , 0 , sizeof(cOrderno));
                  memset(&sHvFzf_Upd , 0 , sizeof(sHvFzf_Upd));
                  memset(&hv_fzf_c , 0 , sizeof(hv_fzf_c));
                  /**end by xyy YHVP**/
                  
    vtcp_log("%s,%d, wp_notpayin->qwtday =[%.8s]",__FILE__,__LINE__,wp_notpayin->qwtday );    
    vtcp_log("%s,%d, wp_notpayin->qorbrno =[%.12s]",__FILE__,__LINE__,wp_notpayin->qorbrno );
    vtcp_log("%s,%d, wp_notpayin->qorderno =[%.8s]",__FILE__,__LINE__,wp_notpayin->qorderno );  
    
                  memcpy(cOr_br_no, wp_notpayin->qorbrno, sizeof(cOr_br_no)-1); /*��ѯ���к�*/
                  memcpy(cOrderno, wp_notpayin->qorderno, sizeof(cOrderno)-1);  /*ԭ��ѯ���*/
                  memcpy(cWt_date, wp_notpayin->qwtday, sizeof(cWt_date)-1);    /*ԭ��ѯ����*/ 
                  
                  memset(&sHvFzf_Upd, 0, sizeof(struct hv_fzf_c));
                  memset(&hv_fzf_c, 0, sizeof(struct hv_fzf_c));                  

                  if( iCmtno != 302 )
                  {
                    vtcp_log("%s,%d,����[%d]����CMT302�鸴����\n",__FILE__,__LINE__,iCmtno);
                    sprintf(acErrMsg,"���˲���CMT302�鸴����");
                    WRITEMSG
                    return(-1);
                  }
                  
                  ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, " otype = '2' and pay_qs_no='%s' and orderno = '%s' and cmtno = '301'", HV_QS_BR_NO, cOrderno);
                  if (ret)
                  {
                      vtcp_log("[%s][%d]����ԭ��ѯ���ļ�¼����!sqlcode=%d\n",__FILE__,__LINE__,ret);
                      WRITEMSG
                      return -1;
                  }
                  
                  ret = Hv_fzf_Fet_Upd( &sHvFzf_Upd, g_pub_tx.reply );
                  if ( ret == 100 )  /* ԭ��ѯ��¼������ */
                  {
                      vtcp_log("[%s] [%d]   ԭ��ѯ��¼������, ����hv_fzf/hv_fzfhtr !",__FILE__,__LINE__ );

                      char Otxamt [16];
                      memset(Otxamt,0,sizeof(Otxamt));
                      
                      hv_fzf_c.otype[0] = '2';
                      hv_fzf_c.lw_ind[0] = '2';   /*������ʶ*/
                      hv_fzf_c.hv_prt_ind[0] = '0'; 
                      
                      memcpy(hv_fzf_c.cmtno,   wp_notpayin->cmtno,   sizeof(hv_fzf_c.cmtno)-1);

                  /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
                  Modified by ChenMing  2006.10.10.                  **/  
                  hv_fzf_c.wt_date = apatoi(wp_notpayin->txday,8);    
                  hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
                  vtcp_log("[%s][%d] hv_fzf_c.wt_date=[%d]    ",__FILE__,__LINE__,hv_fzf_c.wt_date);  
                  vtcp_log("[%s][%d] hv_fzf_c.tx_date=[%d] ",__FILE__,__LINE__,hv_fzf_c.tx_date);  


                      memcpy(hv_fzf_c.sendco,   wp_notpayin->sendco,   sizeof(hv_fzf_c.sendco)-1);
                      memcpy(hv_fzf_c.pay_qs_no,   wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
                      memcpy(hv_fzf_c.or_br_no,   wp_notpayin->orbrno,   sizeof(hv_fzf_c.or_br_no)-1);
                      memcpy(hv_fzf_c.orderno,  wp_notpayin->orderno,   sizeof(hv_fzf_c.orderno)-1);
                      memcpy(hv_fzf_c.receco,   wp_notpayin->receco,   sizeof(wp_notpayin->receco));
                      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
                      memcpy(hv_fzf_c.ac_br_no,    wp_notpayin->acbrno,   sizeof(hv_fzf_c.ac_br_no)-1);
                      hv_fzf_c.req_date = apatoi( wp_notpayin->qwtday,sizeof(wp_notpayin->qwtday));
                      hv_fzf_c.o_wt_date = apatoi(wp_notpayin->owtday,8);
                      memcpy(hv_fzf_c.o_or_br_no,  wp_notpayin->qorbrno,  sizeof(hv_fzf_c.o_or_br_no)-1);
                      memcpy(hv_fzf_c.o_ac_br_no,  wp_notpayin->racbrno,  sizeof(hv_fzf_c.o_ac_br_no)-1);
                      hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
                      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
                      /*ԭ��ѯ���*/
                      memcpy(hv_fzf_c.req_orderno, wp_notpayin->qorderno, sizeof(hv_fzf_c.req_orderno)-1);
                      /*ԭ֧���������*/
                      memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
                      memcpy(Otxamt,wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt));
                      zip_space(Otxamt);
                      str2dbl(Otxamt,strlen(Otxamt),0,&hv_fzf_c.o_tx_amt);  

                      sHvFzf_Upd.res_sts[0] = STAT_LZRECV1 ;         /*  'H' ��ʱ����,������*/            
                      sHvFzf_Upd.hv_fzf_sts[0] = STAT_LZRECV1 ;      /*  'H' ��ʱ����,������*/
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
       
         
                      hv_fzf_c.tx_time = apatoi(cTime,6);
                      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
                      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
                      
                      hv_fzf_c.send_trace_no = apatoi(wp_notpayin->wssrno, sizeof(wp_notpayin->wssrno));
                
                      vtcp_log("[%s][%d] Hv_fzf_Debug() --------------\n",__FILE__,__LINE__);
                      Hv_fzf_Debug(&hv_fzf_c);
                      
                      /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
                      if(strlen(hv_fzf_c.sendco) < 4)
                      {
                          iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
                      }
                      if(strlen(hv_fzf_c.receco) < 4)
                      {
                          iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
                      }
                      /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
                          
                      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
                      if ( ret )
                      {
                        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
                        WRITEMSG
                          return -1;
                      }
                      
                      struct hv_fzfhtr_c hv_fzfhtr;
                      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
                      
                      iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/
                
                      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
                      if(ret)
                      {
                           sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                           WRITEMSG
                           strcpy(g_pub_tx.reply,"HV01");
                           return (-1);
                      }
                      
                      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);  

                  }else if( ret == 0)  /* �ҵ�ԭ301��¼�������ԭ301��¼ */
                  {
                      vtcp_log("[%s] [%d]  �ҵ�ԭ301��¼�������ԭ301��¼ !",__FILE__,__LINE__ );

                      sHvFzf_Upd.res_sts[0] = '9';    /*  ����˵ ��ѯʱҪ��һ����־λ */                
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
                      
                      ret = Hv_fzf_Upd_Upd( sHvFzf_Upd, g_pub_tx.reply );
                      if ( ret )
                      {
                        sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
                        WRITEMSG
                          return -1;
                      }

                  }
                  else   /* ���� */
                  {
                       sprintf( acErrMsg, "ִ��Hv_fzf_Fet_Upd��![%d]", ret );
                      WRITEMSG
                      return -1;                   
                  }
                  
                  Hv_fzf_Clo_Upd();                  

             }
              break;


          case 320:  /* �˻�Ӧ�� */
              {
                  /* ��ԭ��ϵͳ��ͬ���鵽ԭ319��¼�����ԭ319��¼��û�ҵ��Ų���*/
                  char cOr_br_no[13];
                  char cWt_date[9];
                  char cOrderno[13];    
                  struct hv_fzf_c sHvFzf_Upd;            
                  struct hv_fzf_c hv_fzf_c;  
                  
                  /**add by xyy YHBP �����������**/   
                  memset(cOr_br_no , 0 , sizeof(cOr_br_no));
                  memset(cWt_date  , 0 , sizeof(cWt_date));
                  memset(cOrderno  , 0 , sizeof(cOrderno));
                  memset(&sHvFzf_Upd , 0 , sizeof(sHvFzf_Upd));
                  memset(&hv_fzf_c , 0 , sizeof(hv_fzf_c));
                  /**end by xyy YHVP**/   

                  memcpy(cOr_br_no, wp_notpayin->oorbrno, sizeof(cOr_br_no)-1);
                  memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
                  memcpy(cWt_date, wp_notpayin->qwtday, sizeof(cWt_date)-1);    
                  
                  memset(&sHvFzf_Upd, 0, sizeof(struct hv_fzf_c));
                  memset(&hv_fzf_c, 0, sizeof(struct hv_fzf_c));                  

                  if( iCmtno != 320 )
                  {
                    vtcp_log("%s,%d,����[%d]����CMT320�˻�Ӧ����\n",__FILE__,__LINE__,iCmtno);
                    sprintf(acErrMsg,"���˲���CMT320�˻�Ӧ����");
                    WRITEMSG
                    return(-1);
                  }
                  
                  ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, " otype = '2' and pay_qs_no='%s' and orderno = '%s' and cmtno = '319'", HV_QS_BR_NO, cOr_br_no );
                  if (ret)
                  {
                      vtcp_log("[%s][%d]����С��ҵ���˻����뱨�Ĵ���!sqlcode=%d\n",__FILE__,__LINE__,ret);
                      WRITEMSG
                      return -1;
                  }
                  
                  ret = Hv_fzf_Fet_Upd( &sHvFzf_Upd, g_pub_tx.reply );
                  if ( ret == 100 )  /* ԭС��ҵ���˻������¼������ */
                  {
                      vtcp_log("[%s] [%d]   ԭС��ҵ���˻������¼������, ����hv_fzf/hv_fzfhtr !",__FILE__,__LINE__ );

                      char Otxamt [16];
                      memset(Otxamt,0,sizeof(Otxamt));
                      
                      hv_fzf_c.otype[0] = '2';
                      hv_fzf_c.lw_ind[0] = '2';   /*������ʶ*/
                      hv_fzf_c.hv_prt_ind[0] = '0'; 
                      
                      memcpy(hv_fzf_c.cmtno,   wp_notpayin->cmtno,   sizeof(hv_fzf_c.cmtno)-1);

                  /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
                  Modified by ChenMing  2006.10.10.                  **/  
                  hv_fzf_c.wt_date = apatoi(wp_notpayin->txday,8);    
                  hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
                  vtcp_log("[%s][%d] hv_fzf_c.wt_date=[%d]    ",__FILE__,__LINE__,hv_fzf_c.wt_date);  
                  vtcp_log("[%s][%d] hv_fzf_c.tx_date=[%d] ",__FILE__,__LINE__,hv_fzf_c.tx_date);  


                      memcpy(hv_fzf_c.sendco,   wp_notpayin->sendco,   sizeof(hv_fzf_c.sendco)-1);
                      memcpy(hv_fzf_c.pay_qs_no,   wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
                      memcpy(hv_fzf_c.or_br_no,   wp_notpayin->orbrno,   sizeof(hv_fzf_c.or_br_no)-1);
                      memcpy(hv_fzf_c.orderno,  wp_notpayin->orderno,   sizeof(hv_fzf_c.orderno)-1);
                      memcpy(hv_fzf_c.receco,   wp_notpayin->receco,   sizeof(wp_notpayin->receco));
                      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
                      memcpy(hv_fzf_c.ac_br_no,    wp_notpayin->acbrno,   sizeof(hv_fzf_c.ac_br_no)-1);
                      hv_fzf_c.req_date = apatoi( wp_notpayin->qwtday,sizeof(wp_notpayin->qwtday));
                      hv_fzf_c.o_wt_date = apatoi(wp_notpayin->owtday,8);
                      memcpy(hv_fzf_c.o_or_br_no,  wp_notpayin->qorbrno,  sizeof(hv_fzf_c.o_or_br_no)-1);
                      memcpy(hv_fzf_c.o_ac_br_no,  wp_notpayin->racbrno,  sizeof(hv_fzf_c.o_ac_br_no)-1);
                      hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
                      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
                      /*ԭ��ѯ���*/
                      memcpy(hv_fzf_c.req_orderno, wp_notpayin->qorderno, sizeof(hv_fzf_c.req_orderno)-1);
                      /*ԭ֧���������*/
                      memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
                      memcpy(Otxamt,wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt));
                      zip_space(Otxamt);
                      str2dbl(Otxamt,strlen(Otxamt),0,&hv_fzf_c.o_tx_amt);  

                      sHvFzf_Upd.res_sts[0] = STAT_LZRECV1 ;         /*  'H' ��ʱ����,������*/            
                      sHvFzf_Upd.hv_fzf_sts[0] = STAT_LZRECV1 ;      /*  'H' ��ʱ����,������*/
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
         
                      hv_fzf_c.tx_time = apatoi(cTime,6);
                      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
                      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
                      hv_fzf_c.send_trace_no = apatoi(wp_notpayin->wssrno, sizeof(wp_notpayin->wssrno));
                
                      vtcp_log("[%s][%d] Hv_fzf_Debug() --------------\n",__FILE__,__LINE__);
                      Hv_fzf_Debug(&hv_fzf_c);
                      
                  /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
                  if(strlen(hv_fzf_c.sendco) < 4)
                  {
                      iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
                  }
                  if(strlen(hv_fzf_c.receco) < 4)
                  {
                      iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
                  }
                  /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
                      
                      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
                      if ( ret )
                      {
                        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
                        WRITEMSG
                          return -1;
                      }
                      
                      struct hv_fzfhtr_c hv_fzfhtr;
                      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
                      
                      iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/
                
                      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
                      if(ret)
                      {
                           sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                           WRITEMSG
                           strcpy(g_pub_tx.reply,"HV01");
                           return (-1);
                      }
                      
                      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);  

                  }else if( ret == 0)  /* �ҵ�ԭ319��¼�������ԭ319��¼ */
                  {
                    
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);

                      ret = Hv_fzf_Upd_Upd( sHvFzf_Upd, g_pub_tx.reply );
                      if ( ret )
                      {
                        sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
                        WRITEMSG
                          return -1;
                      }

                  }
                  else   /* ���� */
                  {
                       sprintf( acErrMsg, "ִ��Hv_fzf_Fet_Upd��![%d]", ret );
                      WRITEMSG
                      return -1;                   
                  }
                  
                  Hv_fzf_Clo_Upd();                  

             }
              break;      
            
          case 322:  /* ����Ӧ�� */            
              {
                  /* ��ԭ��ϵͳ��ͬ���鵽ԭ321��¼�����ԭ321��¼��û�ҵ��Ų���*/
                  char cOr_br_no[13];
                  char cWt_date[9];
                  char cOrderno[13];    
                  struct hv_fzf_c sHvFzf_Upd;            
                  struct hv_fzf_c hv_fzf_c;     
                  
                  /**add by xyy YHBP �����������**/   
                  memset(cOr_br_no , 0 , sizeof(cOr_br_no));
                  memset(cWt_date  , 0 , sizeof(cWt_date));
                  memset(cOrderno  , 0 , sizeof(cOrderno));
                  memset(&sHvFzf_Upd , 0 , sizeof(sHvFzf_Upd));
                  memset(&hv_fzf_c , 0 , sizeof(hv_fzf_c));
                  /**end by xyy YHVP**/
                  
                  memcpy(cOr_br_no, wp_notpayin->oorbrno, sizeof(cOr_br_no)-1);
                  memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
                  memcpy(cWt_date, wp_notpayin->qwtday, sizeof(cWt_date)-1);    
                  
                  memset(&sHvFzf_Upd, 0, sizeof(struct hv_fzf_c));
                  memset(&hv_fzf_c, 0, sizeof(struct hv_fzf_c));                  

                  if( iCmtno != 322 )
                  {
                    vtcp_log("%s,%d,����[%d]����CMT322����Ӧ��\n",__FILE__,__LINE__,iCmtno);
                    sprintf(acErrMsg,"���˲���CMT322����Ӧ��");
                    WRITEMSG
                    return(-1);
                  }
                  
                  ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, " otype = '2' and pay_qs_no='%s' and orderno = '%s' and cmtno = '321'", HV_QS_BR_NO, cOrderno);
                  if (ret)
                  {
                      vtcp_log("[%s][%d]����С��ҵ���������¼����!sqlcode=%d\n",__FILE__,__LINE__,ret);
                      WRITEMSG
                      return -1;
                  }
                  
                  ret = Hv_fzf_Fet_Upd( &sHvFzf_Upd, g_pub_tx.reply );
                  if ( ret == 100 )  /* ԭ���������¼������ */
                  {
                      vtcp_log("[%s] [%d]   ԭ��ѯ��¼������, ����hv_fzf/hv_fzfhtr !",__FILE__,__LINE__ );

                      char Otxamt [16];
                      memset(Otxamt,0,sizeof(Otxamt));
                      
                      hv_fzf_c.otype[0] = '2';
                      hv_fzf_c.lw_ind[0] = '2';   /*������ʶ*/
                      hv_fzf_c.hv_prt_ind[0] = '0'; 
                      
                      memcpy(hv_fzf_c.cmtno,   wp_notpayin->cmtno,   sizeof(hv_fzf_c.cmtno)-1);

                  /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
                  Modified by ChenMing  2006.10.10.                  **/  
                  hv_fzf_c.wt_date = apatoi(wp_notpayin->txday,8);    
                  hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
                  vtcp_log("[%s][%d] hv_fzf_c.wt_date=[%d]    ",__FILE__,__LINE__,hv_fzf_c.wt_date);  
                  vtcp_log("[%s][%d] hv_fzf_c.tx_date=[%d] ",__FILE__,__LINE__,hv_fzf_c.tx_date);  


                      memcpy(hv_fzf_c.sendco,   wp_notpayin->sendco,   sizeof(hv_fzf_c.sendco)-1);
                      memcpy(hv_fzf_c.pay_qs_no,   wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
                      memcpy(hv_fzf_c.or_br_no,   wp_notpayin->orbrno,   sizeof(hv_fzf_c.or_br_no)-1);
                      memcpy(hv_fzf_c.orderno,  wp_notpayin->orderno,   sizeof(hv_fzf_c.orderno)-1);
                      memcpy(hv_fzf_c.receco,   wp_notpayin->receco,   sizeof(wp_notpayin->receco));
                      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
                      memcpy(hv_fzf_c.ac_br_no,    wp_notpayin->acbrno,   sizeof(hv_fzf_c.ac_br_no)-1);
                      hv_fzf_c.req_date = apatoi( wp_notpayin->qwtday,sizeof(wp_notpayin->qwtday));
                      hv_fzf_c.o_wt_date = apatoi(wp_notpayin->owtday,8);
                      memcpy(hv_fzf_c.o_or_br_no,  wp_notpayin->qorbrno,  sizeof(hv_fzf_c.o_or_br_no)-1);
                      memcpy(hv_fzf_c.o_ac_br_no,  wp_notpayin->racbrno,  sizeof(hv_fzf_c.o_ac_br_no)-1);
                      hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
                      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
                      /*ԭ��ѯ���*/
                      memcpy(hv_fzf_c.req_orderno, wp_notpayin->qorderno, sizeof(hv_fzf_c.req_orderno)-1);
                      /*ԭ֧���������*/
                      memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
                      memcpy(Otxamt,wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt));
                      zip_space(Otxamt);
                      str2dbl(Otxamt,strlen(Otxamt),0,&hv_fzf_c.o_tx_amt);  

                      sHvFzf_Upd.res_sts[0] = STAT_LZRECV1 ;         /*  'H' ��ʱ����,������*/            
                      sHvFzf_Upd.hv_fzf_sts[0] = STAT_LZRECV1 ;      /*  'H' ��ʱ����,������*/
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
       
         
                      hv_fzf_c.tx_time = apatoi(cTime,6);
                      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
                      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
                      
                      hv_fzf_c.send_trace_no = apatoi(wp_notpayin->wssrno, sizeof(wp_notpayin->wssrno));
                
                      vtcp_log("[%s][%d] Hv_fzf_Debug() --------------\n",__FILE__,__LINE__);
                      
                      Hv_fzf_Debug(&hv_fzf_c);
                      
                  /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
                  if(strlen(hv_fzf_c.sendco) < 4)
                  {
                      iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
                  }
                  if(strlen(hv_fzf_c.receco) < 4)
                  {
                      iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
                  }
                  /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
                      
                      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
                      if ( ret )
                      {
                        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
                        WRITEMSG
                          return -1;
                      }
                      
                      struct hv_fzfhtr_c hv_fzfhtr;
                      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
                      
                      iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/
                
                      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
                      if(ret)
                      {
                           sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                           WRITEMSG
                           strcpy(g_pub_tx.reply,"HV01");
                           return (-1);
                      }
                      
                      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);  

                  }else if( ret == 0)  /* �ҵ�ԭ321��¼�������ԭ321��¼ */
                  {
                    
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
                       
                      ret = Hv_fzf_Upd_Upd( sHvFzf_Upd, g_pub_tx.reply );
                      if ( ret )
                      {
                          sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
                          WRITEMSG
                          return -1;
                      }

                  }
                  else   /* ���� */
                  {
                       sprintf( acErrMsg, "ִ��Hv_fzf_Fet_Upd��![%d]", ret );
                      WRITEMSG
                      return -1;                   
                  }
                  
                  Hv_fzf_Clo_Upd();                  

             }
              break;            
            
          case 324:  /* ����֪ͨӦ��--��ԭ��323���� */            
              {
                  /* ��ԭ��ϵͳ��ͬ���鵽ԭ323��¼�����ԭ323��¼��û�ҵ��Ų���*/
                  char cOr_br_no[13];
                  char cWt_date[9];
                  char cOrderno[13];    
                  struct hv_fzf_c sHvFzf_Upd;            
                  struct hv_fzf_c hv_fzf_c;     

                  /**add by xyy YHBP �����������**/   
                  memset(cOr_br_no , 0 , sizeof(cOr_br_no));
                  memset(cWt_date  , 0 , sizeof(cWt_date));
                  memset(cOrderno  , 0 , sizeof(cOrderno));
                  memset(&sHvFzf_Upd , 0 , sizeof(sHvFzf_Upd));
                  memset(&hv_fzf_c , 0 , sizeof(hv_fzf_c));
                  /**end by xyy YHVP**/

                  memcpy(cOr_br_no, wp_notpayin->oorbrno, sizeof(cOr_br_no)-1);
                  memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
                  memcpy(cWt_date, wp_notpayin->qwtday, sizeof(cWt_date)-1);    
                  
                  memset(&sHvFzf_Upd, 0, sizeof(struct hv_fzf_c));
                  memset(&hv_fzf_c, 0, sizeof(struct hv_fzf_c));                  

                  if( iCmtno != 324 )
                  {
                    vtcp_log("%s,%d,����[%d]����CMT324����Ӧ��\n",__FILE__,__LINE__,iCmtno);
                    sprintf(acErrMsg,"���˲���CMT324����Ӧ��");
                    WRITEMSG
                    return(-1);
                  }
                  
                  ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, " otype = '2' and  pay_qs_no='%s'  and orderno = '%s' and cmtno = '323'", HV_QS_BR_NO, cOrderno);
                  if (ret)
                  {
                      vtcp_log("[%s][%d]�����������뱨�Ľ��յǼǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
                      WRITEMSG
                      return -1;
                  }
                  
                  ret = Hv_fzf_Fet_Upd( &sHvFzf_Upd, g_pub_tx.reply );
                  if ( ret == 100 )  /* ԭ���������¼������ */
                  {
                      vtcp_log("[%s] [%d]   ԭ���������¼������, ����hv_fzf/hv_fzfhtr !",__FILE__,__LINE__ );

                      char Otxamt [16];
                      memset(Otxamt,0,sizeof(Otxamt));
                      
                      hv_fzf_c.otype[0] = '2';
                      hv_fzf_c.lw_ind[0] = '2';   /*������ʶ*/
                      hv_fzf_c.hv_prt_ind[0] = '0'; 
                      
                      memcpy(hv_fzf_c.cmtno,   wp_notpayin->cmtno,   sizeof(hv_fzf_c.cmtno)-1);

                  /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
                  Modified by ChenMing  2006.10.10.                  **/  
                  hv_fzf_c.wt_date = apatoi(wp_notpayin->txday,8);    
                  hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
                  vtcp_log("[%s][%d] hv_fzf_c.wt_date=[%d]    ",__FILE__,__LINE__,hv_fzf_c.wt_date);  
                  vtcp_log("[%s][%d] hv_fzf_c.tx_date=[%d] ",__FILE__,__LINE__,hv_fzf_c.tx_date);  

                      memcpy(hv_fzf_c.sendco,   wp_notpayin->sendco,   sizeof(hv_fzf_c.sendco)-1);
                      memcpy(hv_fzf_c.pay_qs_no,   wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
                      memcpy(hv_fzf_c.or_br_no,   wp_notpayin->orbrno,   sizeof(hv_fzf_c.or_br_no)-1);
                      memcpy(hv_fzf_c.orderno,  wp_notpayin->orderno,   sizeof(hv_fzf_c.orderno)-1);
                      memcpy(hv_fzf_c.receco,   wp_notpayin->receco,   sizeof(wp_notpayin->receco));
                      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
                      memcpy(hv_fzf_c.ac_br_no,    wp_notpayin->acbrno,   sizeof(hv_fzf_c.ac_br_no)-1);
                      hv_fzf_c.req_date = apatoi( wp_notpayin->qwtday,sizeof(wp_notpayin->qwtday));
                      hv_fzf_c.o_wt_date = apatoi(wp_notpayin->owtday,8);
                      memcpy(hv_fzf_c.o_or_br_no,  wp_notpayin->qorbrno,  sizeof(hv_fzf_c.o_or_br_no)-1);
                      memcpy(hv_fzf_c.o_ac_br_no,  wp_notpayin->racbrno,  sizeof(hv_fzf_c.o_ac_br_no)-1);
                      hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
                      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
                      /*ԭ��ѯ���*/
                      memcpy(hv_fzf_c.req_orderno, wp_notpayin->qorderno, sizeof(hv_fzf_c.req_orderno)-1);
                      /*ԭ֧���������*/
                      memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
                      memcpy(Otxamt,wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt));
                      zip_space(Otxamt);
                      str2dbl(Otxamt,strlen(Otxamt),0,&hv_fzf_c.o_tx_amt);  

                      sHvFzf_Upd.res_sts[0] = STAT_LZRECV1 ;         /*  'H' ��ʱ����,������*/            
                      sHvFzf_Upd.hv_fzf_sts[0] = STAT_LZRECV1 ;      /*  'H' ��ʱ����,������*/
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
       
         
                      hv_fzf_c.tx_time = apatoi(cTime,6);
                      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
                      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
                      
                      hv_fzf_c.send_trace_no = apatoi(wp_notpayin->wssrno, sizeof(wp_notpayin->wssrno));
                
                      vtcp_log("[%s][%d] Hv_fzf_Debug() --------------\n",__FILE__,__LINE__);
                      
                      Hv_fzf_Debug(&hv_fzf_c);
                      
                  /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
                  if(strlen(hv_fzf_c.sendco) < 4)
                  {
                      iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
                  }
                  if(strlen(hv_fzf_c.receco) < 4)
                  {
                      iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
                  }
                  /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/

                      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
                      if ( ret )
                      {
                        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
                        WRITEMSG
                          return -1;
                      }
                      
                      struct hv_fzfhtr_c hv_fzfhtr;
                      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
                      
                      iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/
                
                      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
                      if(ret)
                      {
                           sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                           WRITEMSG
                           strcpy(g_pub_tx.reply,"HV01");
                           return (-1);
                      }
                      
                      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);  

                  }else if( ret == 0)  /* �ҵ�ԭ323��¼�������ԭ323��¼ */
                  {
                    
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
                       
                      
                      ret = Hv_fzf_Upd_Upd( sHvFzf_Upd, g_pub_tx.reply );
                      if ( ret )
                      {
                        sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
                        WRITEMSG
                          return -1;
                      }

                  }
                  else   /* ���� */
                  {
                       sprintf( acErrMsg, "ִ��Hv_fzf_Fet_Upd��![%d]", ret );
                      WRITEMSG
                      return -1;                   
                  }
                  
                  Hv_fzf_Clo_Upd();                  

             }
              break;  
        

          case 328:  /* ���ҵ��ֹ��Ӧ���� */
              {
                  /* ��ԭ��ϵͳ��ͬ���鵽ԭ327��¼�����ԭ327��¼��û�ҵ��Ų���*/
                  char cOr_br_no[13];
                  char cWt_date[9];
                  char cOrderno[13];    
                  struct hv_fzf_c sHvFzf_Upd;            
                  struct hv_fzf_c hv_fzf_c;     

                  /**add by xyy YHBP �����������**/   
                  memset(cOr_br_no , 0 , sizeof(cOr_br_no));
                  memset(cWt_date  , 0 , sizeof(cWt_date));
                  memset(cOrderno  , 0 , sizeof(cOrderno));
                  memset(&sHvFzf_Upd , 0 , sizeof(sHvFzf_Upd));
                  memset(&hv_fzf_c , 0 , sizeof(hv_fzf_c));
                  /**end by xyy YHVP**/ 

                  memcpy(cOr_br_no, wp_notpayin->oorbrno, sizeof(cOr_br_no)-1);
                  memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
                  memcpy(cWt_date, wp_notpayin->qwtday, sizeof(cWt_date)-1);    
                  
                  memset(&sHvFzf_Upd, 0, sizeof(struct hv_fzf_c));
                  memset(&hv_fzf_c, 0, sizeof(struct hv_fzf_c));                  

                  if( iCmtno != 328 )
                  {
                    vtcp_log("%s,%d,����[%d]����CMT328ֹ��Ӧ����\n",__FILE__,__LINE__,iCmtno);
                    sprintf(acErrMsg,"���˲���CMT328ֹ��Ӧ����");
                    WRITEMSG
                    return(-1);
                  }
                  
                  ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, " otype = '2' and  pay_qs_no='%s'  and orderno = '%s' and cmtno = '%d'", HV_QS_BR_NO, cOrderno,iCmtno);
                  if (ret)
                  {
                      vtcp_log("[%s][%d]����ֹ�����뱨�Ľ��յǼǲ�����!sqlcode=%d\n",__FILE__,__LINE__,ret);
                      WRITEMSG
                      return -1;
                  }
                  
                  ret = Hv_fzf_Fet_Upd( &sHvFzf_Upd, g_pub_tx.reply );
                  if ( ret == 100 )  /* ԭֹ�������¼������ */
                  {
                      vtcp_log("[%s] [%d]   ԭֹ�������¼������, ����hv_fzf/hv_fzfhtr !",__FILE__,__LINE__ );

                      char Otxamt [16];
                      memset(Otxamt,0,sizeof(Otxamt));
                      
                      hv_fzf_c.otype[0] = '2';
                      hv_fzf_c.lw_ind[0] = '2';   /*������ʶ*/
                      hv_fzf_c.hv_prt_ind[0] = '0'; 
                      
                      memcpy(hv_fzf_c.cmtno,   wp_notpayin->cmtno,   sizeof(hv_fzf_c.cmtno)-1);

                  /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
                  Modified by ChenMing  2006.10.10.                  **/  
                  hv_fzf_c.wt_date = apatoi(wp_notpayin->txday,8);    
                  hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
                  vtcp_log("[%s][%d] hv_fzf_c.wt_date=[%d]    ",__FILE__,__LINE__,hv_fzf_c.wt_date);  
                  vtcp_log("[%s][%d] hv_fzf_c.tx_date=[%d] ",__FILE__,__LINE__,hv_fzf_c.tx_date);  


                      memcpy(hv_fzf_c.sendco,   wp_notpayin->sendco,   sizeof(hv_fzf_c.sendco)-1);
                      memcpy(hv_fzf_c.pay_qs_no,   wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
                      memcpy(hv_fzf_c.or_br_no,   wp_notpayin->orbrno,   sizeof(hv_fzf_c.or_br_no)-1);
                      memcpy(hv_fzf_c.orderno,  wp_notpayin->orderno,   sizeof(hv_fzf_c.orderno)-1);
                      memcpy(hv_fzf_c.receco,   wp_notpayin->receco,   sizeof(wp_notpayin->receco));
                      memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
                      memcpy(hv_fzf_c.ac_br_no,    wp_notpayin->acbrno,   sizeof(hv_fzf_c.ac_br_no)-1);
                      hv_fzf_c.req_date = apatoi( wp_notpayin->qwtday,sizeof(wp_notpayin->qwtday));
                      hv_fzf_c.o_wt_date = apatoi(wp_notpayin->owtday,8);
                      memcpy(hv_fzf_c.o_or_br_no,  wp_notpayin->qorbrno,  sizeof(hv_fzf_c.o_or_br_no)-1);
                      memcpy(hv_fzf_c.o_ac_br_no,  wp_notpayin->racbrno,  sizeof(hv_fzf_c.o_ac_br_no)-1);
                      hv_fzf_c.o_tx_type[0] = wp_notpayin->otxtype;
                      hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
                      /*ԭ��ѯ���*/
                      memcpy(hv_fzf_c.req_orderno, wp_notpayin->qorderno, sizeof(hv_fzf_c.req_orderno)-1);
                      /*ԭ֧���������*/
                      memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
                      memcpy(Otxamt,wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt));
                      zip_space(Otxamt);
                      str2dbl(Otxamt,strlen(Otxamt),0,&hv_fzf_c.o_tx_amt);  

                      sHvFzf_Upd.res_sts[0] = STAT_LZRECV1 ;         /*  'H' ��ʱ����,������*/            
                      sHvFzf_Upd.hv_fzf_sts[0] = STAT_LZRECV1 ;      /*  'H' ��ʱ����,������*/
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
       
         
                      hv_fzf_c.tx_time = apatoi(cTime,6);
                      memcpy(hv_fzf_c.br_no,cBrno,sizeof(hv_fzf_c.br_no)-1);
                      memcpy(hv_fzf_c.tel,cTlrno,sizeof(hv_fzf_c.tel)-1);
                      
                      hv_fzf_c.send_trace_no = apatoi(wp_notpayin->wssrno, sizeof(wp_notpayin->wssrno));
                
                      vtcp_log("[%s][%d] Hv_fzf_Debug() --------------\n",__FILE__,__LINE__);
                      
                      Hv_fzf_Debug(&hv_fzf_c);
                      
                  /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
                  if(strlen(hv_fzf_c.sendco) < 4)
                  {
                       iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
                  }
                  if(strlen(hv_fzf_c.receco) < 4)
                  {
                       iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
                  }
                  /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
                      
                      ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
                      if ( ret )
                      {
                        sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
                        WRITEMSG
                          return -1;
                      }
                      
                      struct hv_fzfhtr_c hv_fzfhtr;
                      memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
                      
                      iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr�Ŀ���***/
                
                      ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
                      if(ret)
                      {
                           sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                           WRITEMSG
                           strcpy(g_pub_tx.reply,"HV01");
                           return (-1);
                      }
                      
                      vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);  

                  }else if( ret == 0)  /* �ҵ�ԭ301��¼�������ԭ301��¼ */
                  {
                    
                      sHvFzf_Upd.res_date = apatoi(wp_notpayin->txday,8);
                      memcpy(sHvFzf_Upd.res_br_no, wp_notpayin->orbrno, sizeof(sHvFzf_Upd.res_br_no)-1);
                      memcpy(sHvFzf_Upd.res_orderno, wp_notpayin->orderno, sizeof(sHvFzf_Upd.res_orderno)-1);
                      memcpy(sHvFzf_Upd.res_content, wp_notpayin->content, sizeof(sHvFzf_Upd.res_content)-1);
                       
                      
                      ret = Hv_fzf_Upd_Upd( sHvFzf_Upd, g_pub_tx.reply );
                      if ( ret )
                      {
                        sprintf( acErrMsg, "�޸ķ�֧���ǼǱ���[%d]", ret );
                        WRITEMSG
                          return -1;
                      }

                  }
                  else   /* ���� */
                  {
                       sprintf( acErrMsg, "ִ��Hv_fzf_Fet_Upd��![%d]", ret );
                      WRITEMSG
                      return -1;                   
                  }
                  
                  Hv_fzf_Clo_Upd();                  

             }
              break;
        
        
       /* �±����������뱨�ģ���Ҫֱ�Ӳ������ */

      case 319: /* �����˻� */
      case 325: /* ����֪ͨ����--ֱ�Ӳ������ */
      case 327: /* ���ҵ��ֹ�����뱨�� */
        {
             struct hv_fzf_c    hv_fzf_c;
             char Otxamt [16];
             memset(Otxamt,0,sizeof(Otxamt));
             memset(&hv_fzf_c, '\0', sizeof(hv_fzf_c));

             hv_fzf_c.otype[0] = '2';
             hv_fzf_c.lw_ind[0] = '2';   /*������ʶ*/
             hv_fzf_c.hv_prt_ind[0] = '0'; 

             
             memcpy(hv_fzf_c.cmtno, wp_notpayin->cmtno, sizeof(hv_fzf_c.cmtno)-1);
             memcpy(hv_fzf_c.or_br_no,wp_notpayin->oorbrno,sizeof(hv_fzf_c.or_br_no)-1);

            /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
            Modified by ChenMing  2006.10.10.                  **/  
            hv_fzf_c.wt_date = apatoi(wp_notpayin->txday,8);    
            hv_fzf_c.tx_date = apatoi(pcLvGetDay(),8);
            vtcp_log("[%s][%d] hv_fzf_c.wt_date=[%d]    ",__FILE__,__LINE__,hv_fzf_c.wt_date);  
            vtcp_log("[%s][%d] hv_fzf_c.tx_date=[%d] ",__FILE__,__LINE__,hv_fzf_c.tx_date);  

             hv_fzf_c.req_date=hv_fzf_c.wt_date;
             
             memcpy(hv_fzf_c.sendco, wp_notpayin->sendco, sizeof(hv_fzf_c.sendco)-1);
             memcpy(hv_fzf_c.orderno,wp_notpayin->orderno,sizeof(hv_fzf_c.orderno)-1);
             memcpy(hv_fzf_c.pay_qs_no, wp_notpayin->payqsactno, sizeof(hv_fzf_c.pay_qs_no)-1);
             memcpy(hv_fzf_c.or_br_no,  wp_notpayin->orbrno, sizeof(hv_fzf_c.or_br_no)-1);
            /*BEGIN NEWYX add by LiuHuafeng 2007-4-4 5:56*/
            if(strlen(hv_fzf_c.or_br_no)==0)
            {
                memcpy(hv_fzf_c.or_br_no,hv_fzf_c.pay_qs_no,sizeof(hv_fzf_c.or_br_no)-1);
            }
            /* END NEWYX by LiuHuafeng 2007-4-4 5:56 */
             memcpy(hv_fzf_c.orderno,wp_notpayin->orderno,sizeof(hv_fzf_c.orderno)-1);
             memcpy(hv_fzf_c.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
             memcpy(hv_fzf_c.cash_qs_no, wp_notpayin->cashqsactno, sizeof(hv_fzf_c.cash_qs_no)-1);
             memcpy(hv_fzf_c.ac_br_no, wp_notpayin->acbrno, sizeof(hv_fzf_c.ac_br_no)-1);
            /*BEGIN NEWYX add by LiuHuafeng 2007-4-4 5:56*/
            if(strlen(hv_fzf_c.ac_br_no)==0)
            {
                memcpy(hv_fzf_c.ac_br_no,hv_fzf_c.cash_qs_no,sizeof(hv_fzf_c.ac_br_no)-1);
            }

            hv_fzf_c.tx_time = apatoi(cTime,8);
             memcpy(hv_fzf_c.br_no, cBrno, sizeof(hv_fzf_c.br_no)-1);
             memcpy(hv_fzf_c.tel, cTlrno, sizeof(hv_fzf_c.tel)-1);
             hv_fzf_c.trace_no = apatoi(cTraceno,6);
            /* memcpy(hv_fzf_c.o_cur_ind, wp_notpayin->cour, sizeof(hv_fzf_c.o_cur_ind)-1);   */
             memcpy(hv_fzf_c.o_cur_ind, "RMB", sizeof(hv_fzf_c.o_cur_ind)-1);   /*����д����*/
             hv_fzf_c.hv_fzf_sts[0] = wp_notpayin->stat;
             hv_fzf_c.o_wt_date=apatoi(wp_notpayin->owtday,8);

             /* for ����*/             
             hv_fzf_c.o_tx_date = hv_fzf_c.o_wt_date;
             memcpy(hv_fzf_c.o_or_br_no, hv_fzf_c.or_br_no,sizeof(hv_fzf_c.o_or_br_no)-1); 
             memcpy(hv_fzf_c.o_ac_br_no, hv_fzf_c.ac_br_no,sizeof(hv_fzf_c.o_ac_br_no)-1);
             memcpy(hv_fzf_c.o_orderno, wp_notpayin->oorderno, sizeof(hv_fzf_c.o_orderno)-1);
             memcpy(hv_fzf_c.o_cmtno, wp_notpayin->opkgno, sizeof(hv_fzf_c.o_cmtno)-1);   
             hv_fzf_c.o_pack_date = apatoi(wp_notpayin->opackday,8);   
             memcpy(hv_fzf_c.o_packid, wp_notpayin->opackid, sizeof(hv_fzf_c.o_packid)-1);               
    
             memcpy(hv_fzf_c.otxnum, wp_notpayin->otxnum, sizeof(hv_fzf_c.otxnum)-1);               
             
             memcpy(Otxamt,wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt));
             zip_space(Otxamt); 
             /* hv_fzf_c.o_tx_amt =   
             hv_fzf_c.o_tx_type  ??    */

             memcpy(hv_fzf_c.req_content,wp_notpayin->content,sizeof(hv_fzf_c.req_content)-1);
             memcpy(hv_fzf_c.req_br_no,hv_fzf_c.or_br_no,sizeof(hv_fzf_c.req_br_no)-1);
             memcpy(hv_fzf_c.req_orderno,hv_fzf_c.orderno,sizeof(hv_fzf_c.req_orderno)-1);             
             hv_fzf_c.reqtype[0] = wp_notpayin->rettype;

             /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
             if(strlen(hv_fzf_c.sendco) < 4)
             {
                 iGetSendco(hv_fzf_c.sendco, hv_fzf_c.pay_qs_no);
             }
             if(strlen(hv_fzf_c.receco) < 4)
             {
                 iGetSendco(hv_fzf_c.receco, hv_fzf_c.cash_qs_no);
             }
             /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
             
             /* Hv_fzf_Debug(&hv_fzf_c); */
             ret = Hv_fzf_Ins(hv_fzf_c, g_pub_tx.reply);
             if ( ret )
             {
                 sprintf( acErrMsg, "ִ��Hv_fzf_Ins����!ret=[%d]", ret );
                 strcpy( g_pub_tx.reply, "EP010" );
                 WRITEMSG
                 return -1;
             }
             
             struct hv_fzfhtr_c hv_fzfhtr;
             memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
             
             iLvFzfToFzfhtr(&hv_fzf_c,&hv_fzfhtr);   /** ���µ� hv_fzf ->hv_fzfhtr�Ŀ���***/
       
             ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
             if(ret)
             {
                     sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!ret==[%d]\n",ret);
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"HV01");
                     return (-1);
             }
             
             vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);        
        }
        break;

      default:  
             sprintf(acErrMsg,"cmtno���� ������  iCmtno==[%d]\n",iCmtno);
             WRITEMSG
             return -1;

      }
  }

  if(iPkgno > 0)   /*��pkg����*/
  {
      vtcp_log("[%s][%d]+++++++++++++ִ�е���������!+++ipkgno=[%d]\n",__FILE__,__LINE__,iPkgno);
    
      switch(iPkgno)
      {
      case 12: /*����ͨ����Ϣ���յǼǲ�*/
      case 13: /*����ʵʱ��Ϣҵ����յǼǲ�*/
        {
          struct lv_pkgreg_c  sLv_pkgreg_r;
          memset(&sLv_pkgreg_r, 0, sizeof(sLv_pkgreg_r));
    
        vtcp_log("[%s][%d]+++++++++++++ִ�е���������!+++\n",__FILE__,__LINE__);
          memcpy(sLv_pkgreg_r.pkgno,  wp_notpayin->pkgno,  sizeof(sLv_pkgreg_r.pkgno)-1);

          sLv_pkgreg_r.wt_date = apatoi(wp_notpayin->txday, 8);
          
             /** ��ϵͳ��ί�����ڷŵ� tx_date �У����ڷ���wt_date�У����Ի�һ�� 
            Modified by ChenMing  2006.10.10.                  **/  
            sLv_pkgreg_r.wt_date = apatoi(wp_notpayin->txday,8);    
            sLv_pkgreg_r.in_date = apatoi(pcLvGetDay(),8);
            vtcp_log("[%s][%d] sLv_pkgreg_r.wt_date=[%d]    ",__FILE__,__LINE__,sLv_pkgreg_r.wt_date);  
            vtcp_log("[%s][%d] sLv_pkgreg_r.tx_date=[%d] ",__FILE__,__LINE__,sLv_pkgreg_r.in_date);  

          sLv_pkgreg_r.pack_date = sLv_pkgreg_r.in_date;
          
          
          
          memcpy(sLv_pkgreg_r.orderno,wp_notpayin->orderno,sizeof(sLv_pkgreg_r.orderno)-1);
          get_zd_long(DC_TIME,&sLv_pkgreg_r.tx_time);
          memcpy(sLv_pkgreg_r.txnum,  wp_notpayin->txnum,  sizeof(sLv_pkgreg_r.txnum)-1);
          memcpy(sLv_pkgreg_r.or_br_no, wp_notpayin->orbrno, sizeof(sLv_pkgreg_r.or_br_no)-1);
          memcpy(sLv_pkgreg_r.pay_qs_no, wp_notpayin->payqsactno, sizeof(sLv_pkgreg_r.pay_qs_no)-1);
          if(strlen(sLv_pkgreg_r.pay_qs_no)==0)/**add by xyy 2009-7-17 22:50:51**/
          {
              memset(&sHv_uniontab , 0 , sizeof(sHv_uniontab));
              g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&sHv_uniontab,"or_br_no='%s' ",sLv_pkgreg_r.or_br_no );
              if (g_reply_int)
              {
                  vtcp_log("[%s][%d]��ѯ�Է��������ĺ������\n",__FILE__,__LINE__);
                  return(99);
              }
              memcpy(sLv_pkgreg_r.pay_qs_no,sHv_uniontab.qs_no,sizeof(sLv_pkgreg_r.pay_qs_no)-1);
              vtcp_log("[%s][%d]pay_qs_no==[%s]",__FILE__,__LINE__,sLv_pkgreg_r.pay_qs_no);
          }
          memcpy(sLv_pkgreg_r.sendco, wp_notpayin->sendco, sizeof(sLv_pkgreg_r.sendco)-1);
          memcpy(sLv_pkgreg_r.ac_br_no, wp_notpayin->acbrno, sizeof(sLv_pkgreg_r.ac_br_no)-1);
          memcpy(sLv_pkgreg_r.cash_qs_no, wp_notpayin->cashqsactno, sizeof(sLv_pkgreg_r.cash_qs_no)-1);
          if(strlen(sLv_pkgreg_r.cash_qs_no)==0)/**add by xyy 2009-7-17 22:50:51**/
          {
              memset(&sHv_uniontab , 0 , sizeof(sHv_uniontab));
              g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&sHv_uniontab,"or_br_no='%s' ",sLv_pkgreg_r.ac_br_no );
              if (g_reply_int)
              {
                  vtcp_log("[%s][%d]��ѯ�Է��������ĺ������\n",__FILE__,__LINE__);
                  return(99);
              }
              memcpy(sLv_pkgreg_r.cash_qs_no,sHv_uniontab.qs_no,sizeof(sLv_pkgreg_r.cash_qs_no)-1);
              vtcp_log("[%s][%d]cash_qs_no==[%s]",__FILE__,__LINE__,sLv_pkgreg_r.cash_qs_no);

          }
          memcpy(sLv_pkgreg_r.receco, wp_notpayin->receco, sizeof(wp_notpayin->receco));
          /*������������*/
          sLv_pkgreg_r.lv_o_sts[0] = wp_notpayin->datatype;
          memcpy(sLv_pkgreg_r.lv_brf, wp_notpayin->content, sizeof(sLv_pkgreg_r.lv_brf)-1);
          if (!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN) || !memcmp(wp_notpayin->txnum,TXNUM_DF,TXNUM_LEN))
          {
    /** NOTE һ�¸�Ϊд�븽����
            if (!memcmp(wp_notpayin->txnum,TXNUM_DS,TXNUM_LEN))
              sLv_pkgreg_r.sfflag[0] = '0';
            else
              sLv_pkgreg_r.sfflag[0] = '1';
            memcpy(sLv_pkgreg_r.sfunit, wp_notpayin->unit,   sizeof(sLv_pkgreg_r.sfunit)-1);
            memcpy(sLv_pkgreg_r.sftype, wp_notpayin->sftype, sizeof(sLv_pkgreg_r.sftype)-1);
    *****************************/
          }
          if (!memcmp(wp_notpayin->txnum,TXNUM_ZHCX,TXNUM_LEN) || !memcmp(wp_notpayin->txnum,TXNUM_CXYD,TXNUM_LEN))
          {
          vtcp_log("[%s][%d]---------wp_notpayin->actno=[%s]\n", __FILE__,__LINE__,wp_notpayin->actno);
          vtcp_log("[%s][%d]---------wp_notpayin->name=[%s]\n",__FILE__,__LINE__,wp_notpayin->name);
    /* NOTE ���¸�Ϊд�븽����
            sLv_pkgreg_r.qacttype[0] = wp_notpayin->acttype;
            memcpy(sLv_pkgreg_r.qactno,   wp_notpayin->actno, sizeof(sLv_pkgreg_r.qactno)-1);
            memcpy(sLv_pkgreg_r.qname,   wp_notpayin->name,   sizeof(sLv_pkgreg_r.qname)-1);
            sLv_pkgreg_r.qpswdcd[0] = wp_notpayin->pswdcd;
            memcpy(sLv_pkgreg_r.qpasswd, wp_notpayin->pascode,   sizeof(sLv_pkgreg_r.qpasswd)-1);
            sLv_pkgreg_r.qtype[0] = wp_notpayin->qtype;
    
            memcpy(sLv_pkgreg_r.opackday,wp_notpayin->opackday, sizeof(sLv_pkgreg_r.opackday)-1);
            memcpy(sLv_pkgreg_r.opackid,  wp_notpayin->opackid,  sizeof(sLv_pkgreg_r.opackid)-1);
            str2dbl(wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt),0,&sLv_pkgreg_r.qavbal);
    *********************************/
          }
          memcpy(sLv_pkgreg_r.addid,  wp_notpayin->addid,  sizeof(sLv_pkgreg_r.addid)-1);
          get_zd_data(DC_TX_BR_NO  ,sLv_pkgreg_r.br_no     );
          get_zd_data(DC_TEL       ,sLv_pkgreg_r.input_tel   );
          get_zd_data(DC_TRACE_NO,sLv_pkgreg_r.input_trace_no);
          memcpy(sLv_pkgreg_r.packid,wp_notpayin->opackid,sizeof(sLv_pkgreg_r.packid)-1);
        vtcp_log("[%s][%d]+++++ִ�е�����+++++\n",__FILE__,__LINE__);
    
          /*���Ӽ���ʱ��״̬*/
          if (memcmp(wp_notpayin->optype, OPCD_LR, 2) == 0)
            sLv_pkgreg_r.lv_sts[0] = STAT_WZLR; 
          else if (memcmp(wp_notpayin->optype, OPCD_SEND, 2) == 0)
            sLv_pkgreg_r.lv_sts[0] = STAT_WZSEND;
          else
            sLv_pkgreg_r.lv_sts[0]=STAT_LZRECV1; /* add by Liuhuafeng 20060406 */
            
          sLv_pkgreg_r.beg_sts[0] =  sLv_pkgreg_r.lv_sts[0];
          sLv_pkgreg_r.lw_ind[0] ='2';  
            
    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    if(strlen(sLv_pkgreg_r.sendco) < 4)
    {
        iGetSendco(sLv_pkgreg_r.sendco, sLv_pkgreg_r.pay_qs_no);
    }
    if(strlen(sLv_pkgreg_r.receco) < 4)
    {
        iGetSendco(sLv_pkgreg_r.receco, sLv_pkgreg_r.cash_qs_no);
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
            
          /*�������ݳ��Ⱥ���������*/
          ret=Lv_pkgreg_Ins( sLv_pkgreg_r , g_pub_tx.reply );
          if (ret != DB_OK)
          {
            vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,ret);
            Lv_pkgreg_Debug(&sLv_pkgreg_r);
            sprintf(acErrMsg,"������Ϣ���յǼǲ�����!",40);
            return ret;
          }
        }
        break;
      }
  }
  return 0;
}


/*��ѯ��ĳһ����԰��������������������״̬����Ҫ���ҵ��*/
/************************************************/
/*  ������:notpayin_to_table_msg(&lv)  */
/*  ��  ��:���Ľṹ        */
/*  dxy   ���ʱ��������      */
/************************************************/
int notpayin_to_table_msg(TLVPACKAGE *lv)
{
  int   iRet=0;
  int   ret=0;
  int   cDtlid=0;
  char  Txamt    [16];
  char  cTxamt    [19];
  char  cTmpstr    [64];
  char  cOrderno    [9];
  char  cPackdate    [9];
  char  cWtdate    [9];
  char  cZcdate    [9];
  char  cOwtdate    [9];
  char  cQsdate     [9];
  double dTxamt=0.0;
    char cZccc [3];
    char cZcjdlx[2];
  char tel[7];    
  
  struct  lv_c254r_c  lv_c254r;
  struct  hv_fzf_c  hv_fzf;
  struct  lv_lbctl_c   lv_lbctl;
  struct  lv_wbctl_c   lv_wbctl;

      
  vtcp_log("[%s][%d][%s] \n",__FILE__,__LINE__,lv->CMTCode);
  memset(cTmpstr     ,0   ,sizeof(cTmpstr  ));
  memset(cPackdate    ,0  ,sizeof(cPackdate ));
  memset(cWtdate    ,0  ,sizeof(cWtdate  ));
  memset(cZcdate    ,0  ,sizeof(cZcdate  ));
  memset(cOwtdate      ,0  ,sizeof(cOwtdate));
  memset(cQsdate      ,0  ,sizeof(cQsdate  ));
  memset(tel      ,0  ,sizeof(tel  ));
    
  memset(Txamt    ,'\0'  ,sizeof(Txamt  ));
  memset(cTxamt    ,'\0'  ,sizeof(cTxamt  ));
  memset(cOrderno    ,'\0'  ,sizeof(cOrderno));
  memset(&lv_c254r  ,'\0'  ,sizeof(lv_c254r));
  memset(&hv_fzf    ,'\0'  ,sizeof(hv_fzf));
  memset(&lv_lbctl  ,'\0'  ,sizeof(lv_lbctl));
  memset(&lv_wbctl  ,'\0'  ,sizeof(lv_wbctl));
  
  
  vtcp_log("[%s][%d][%s] \n",__FILE__,__LINE__,lv->CMTCode);
  if (memcmp(lv->CMTCode,"254",sizeof(lv->CMTCode))==0)
  {

    memset(cZccc     , 0, sizeof(cZccc));
    memset(cZcjdlx   , 0, sizeof(cZcjdlx));
    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
    memcpy(lv_c254r.cmtno, lv->CMTCode, sizeof(lv_c254r.cmtno)-1);
    lv_c254r.in_date = apatoi(pcLvGetDay(),8);
    lv_get_tag(lv,"30A",cWtdate);
    lv_c254r.wt_date = apatoi(cWtdate,8);
    /** ������ϸ�� **/
    cDtlid = sql_max_int("lv_c254r", "dtlid", "wt_date = %ld ",lv_c254r.wt_date);
    
    if (memcmp(g_pub_tx.reply, "D101", 4)==0)
      g_reply_int = cDtlid;
    else
      g_reply_int = DB_OK;
    if (g_reply_int!=DB_OK)
    {
      vtcp_log("%s,%d,g_reply_int=[%d]",__FILE__,__LINE__,g_reply_int);
      lv_c254r.dtlid = 1;
    }else
    {
      lv_c254r.dtlid = cDtlid+1;
    }

    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
    lv_get_tag(lv,"CC3",lv_c254r.o_pay_qs_no);
    lv_get_tag(lv,"CEK",lv_c254r.zc_type);
    lv_get_tag(lv,"BS1",lv_c254r.zc_node);
    lv_get_tag(lv,"BS2",cZcdate);
    lv_c254r.zc_date = apatoi(cZcdate,8);
    lv_get_tag(lv,"BS3",lv_c254r.zc_id);
    lv_get_tag(lv,"90C",lv_c254r.crdb);
    lv_get_tag(lv,"32A",cTxamt);
    memcpy(Txamt,cTxamt+3,sizeof(Txamt)-1);
    lv_c254r.zc_amt=atof(Txamt);
    lv_get_tag(lv,"BS5",cQsdate);
    lv_c254r.qs_date = apatoi(cQsdate,8);
    lv_get_tag(lv,"CIC",lv_c254r.qs_sts);
    vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
    if (!memcmp(lv_c254r.qs_sts,"0",1))
      lv_c254r.sts[0]=STAT_LZRECV1;
    else
      lv_c254r.sts[0]= STAT_LZRECV1;
    memcpy(cZccc     , lv_c254r.zc_id, sizeof(cZccc)-1);
    memcpy(cZcjdlx   , lv_c254r.zc_node, sizeof(cZcjdlx)-1);

   /*******************  ���濪ʼ�����������Ʊ�  ********************/
    g_reply_int =  Lv_lbctl_Dec_Upd( g_pub_tx.reply ,"zcnode= '%c' and zc_date=%ld and zcid='%s' and packstat='03'",cZcjdlx[0],lv_c254r.zc_date,cZccc);
    vtcp_log("[%s][%d]g_reply_int=====[%d]",__FILE__,__LINE__,g_reply_int);
    if( g_reply_int)
    {
      vtcp_log("[%s][%d]�����α�Lv_lbctl_Dec_Sel OVER",__FILE__,__LINE__);
      sprintf(acErrMsg,"�����α����![%s][%d]",__FILE__,__LINE__);
      set_zd_data(DC_GET_MSG,"�����α����");
      WRITEMSG
      return (-1);
    }
    
    while(1)
    {
        g_reply_int = Lv_lbctl_Fet_Upd(&lv_lbctl,g_pub_tx.reply);
        vtcp_log("[%s][%d]g_reply_int===[%d]",__FILE__,__LINE__,g_reply_int);
        if( g_reply_int == 100 )   /* ������� */
        {
           break;
        }else  if( g_reply_int && g_reply_int != 100)
        {
          sprintf(acErrMsg,"���ݿ����![%s][%d]",__FILE__,__LINE__);
          set_zd_data(DC_GET_MSG,"���ݿ����");
          WRITEMSG
          return (-1);
        }    
        
         lv_lbctl.qs_date = lv_c254r.qs_date;
         memcpy(lv_lbctl.packstat,"05",sizeof(lv_lbctl.packstat)-1);
         g_reply_int = Lv_lbctl_Upd_Upd(lv_lbctl,g_pub_tx.reply);  
         if( g_reply_int)
         {
           vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
           sprintf( acErrMsg,"���ݿ����%s,%diERR=[%d]",__FL__,g_reply_int);
           set_zd_data(DC_GET_MSG,"���ݿ����");
           WRITEMSG
           return (-1);
         }
    }
     Lv_lbctl_Clo_Upd( );
     vtcp_log("[%s][%d]  ======  �������Ʊ�������  ====== ",__FILE__,__LINE__);
        
   /*******************  ���濪ʼ�����������Ʊ�  ********************/
        
    g_reply_int =  Lv_wbctl_Dec_Upd ( g_pub_tx.reply ,"zcnode= '%c' and zc_date=%ld and zcid='%s' and packstat='03' ",cZcjdlx[0],lv_c254r.zc_date,cZccc);
    vtcp_log("[%s][%d]g_reply_int=====[%d]",__FILE__,__LINE__,g_reply_int);
    if( g_reply_int)
    {
      vtcp_log("[%s][%d]�����α�Lv_wbctl_Dec_Upd ����",__FILE__,__LINE__);
      sprintf(acErrMsg,"�����α����![%s][%d]",__FILE__,__LINE__);
      set_zd_data(DC_GET_MSG,"�����α����");
      WRITEMSG
      return (-1);
    }
    
    while(1)
    {
        g_reply_int = Lv_wbctl_Fet_Upd(&lv_wbctl,g_pub_tx.reply);
        vtcp_log("[%s][%d]g_reply_int====[%d]",__FILE__,__LINE__,g_reply_int);
        if( g_reply_int == 100 )   /* ������� */
        {
           break;
        }else if( g_reply_int)
        {
          sprintf(acErrMsg,"���ݿ����![%s][%d]",__FILE__,__LINE__);
          set_zd_data(DC_GET_MSG,"���ݿ����");
          WRITEMSG
          return (-1);
          
        }

        lv_wbctl.qs_date = lv_c254r.qs_date;
        memcpy(lv_wbctl.packstat,"05",sizeof(lv_wbctl.packstat)-1);
        g_reply_int = Lv_wbctl_Upd_Upd(lv_wbctl,g_pub_tx.reply);  
        if( g_reply_int)
        {
          vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
          sprintf( acErrMsg,"���ݿ����%s,%d ERR=[%d]",__FL__,g_reply_int);
          set_zd_data(DC_GET_MSG,"���ݿ����");
          WRITEMSG
          return (-1);
        }
    }         
     Lv_wbctl_Clo_Upd( );        
     vtcp_log("[%s][%d]  ======  �������Ʊ�������  ====== ",__FILE__,__LINE__);

   /*************���濪ʼ����С�����������֪ͨ�ǼǱ�  ***************/
   
    g_reply_int = Lv_c254r_Ins(lv_c254r,g_pub_tx.reply);
    if( g_reply_int)
    {
      vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
      sprintf( acErrMsg,"�����¼����%s,%d ERR=[%d]",__FL__,g_reply_int);
      set_zd_data(DC_GET_MSG,"����һ���¼�¼����");
      WRITEMSG
      return (-1);
    }
    /** ���¶���״̬ by LiuHuafeng 2006-11-2 15:30 */
    if(lv_c254r.zc_amt>0.001)
      iUpdLvCheckstat('1');
    /* ���¶��ʱ�־���� */
    
     vtcp_log("[%s][%d]  ======  ����С�����������֪ͨ�ǼǱ�ɹ�  ====== ",__FILE__,__LINE__);
    
    /* add by LiuHuafeng ����������254������Ľ����־ */
    /* 0720 ָ��Ϊ41403�Ľ����־ 0660 Ϊ110�Ľ����־ 
    if(lv_c254r.crdb[0]=='1'){
      set_zd_data("0660","1");
      set_zd_data("0720","2");
    }
    else
    {
      set_zd_data("0660","2");
      set_zd_data("0720","1");
    }
    dTxamt=lv_c254r.zc_amt/100.0;
    set_zd_double("0400",dTxamt);*/
    /** end by LiuHuafeng 20061014 */
    vtcp_log("[%s][%d] 254���Ľ��׽����%f \n",__FILE__,__LINE__,dTxamt);
  }
  else  if(memcmp(lv->CMTCode,"920",sizeof(lv->CMTCode))==0)
  {
    struct hv_fzfhtr_c hv_fzfhtr;
    memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
    memcpy(hv_fzf.cmtno, lv->CMTCode, sizeof(hv_fzf.cmtno)-1);
    hv_fzf.tx_date = apatoi(pcLvGetDay(), 8);
    lv_get_tag(lv,"30A",cWtdate);
    hv_fzf.wt_date = apatoi(cWtdate,8);
    lv_get_tag(lv,"C36",hv_fzf.or_br_no);
    lv_get_tag(lv,"C37",hv_fzf.ac_br_no);
    lv_get_tag(lv,"02D",hv_fzf.o_cmtno);
    
    lv_get_tag(lv,"30I",cPackdate);
    hv_fzf.o_pack_date = apatoi(cPackdate,8);
    memset(cTmpstr , 0 , sizeof(cTmpstr));
    lv_get_tag(lv,"0BE",cTmpstr);
    lv_orderno_add0(cTmpstr,sizeof(hv_fzf.o_packid)-1,hv_fzf.o_packid);
    
    lv_get_tag(lv,"BS1",hv_fzf.filler);/**zcnode����ڵ�����1λ**/
    lv_get_tag(lv,"BS2",hv_fzf.filler+1);/***zcday��������8λ***/
    lv_get_tag(lv,"BS3",hv_fzf.filler+9);/***�����8λ***/
    lv_get_tag(lv,"CIB",hv_fzf.filler+17);/***������״̬2λ***/
    lv_get_tag(lv,"C42",hv_fzf.filler+19);/**���Ĵ�����8λ**/
    lv_get_tag(lv,"CCO",hv_fzf.filler+27);/**ԭ������������**/
    hv_fzf.lw_ind[0]='2';
    hv_fzf.otype[0]='2';
    hv_fzf.hv_fzf_sts[0]= STAT_LZRECV1;
    
    /**Begin:  Added by Chen Ming   2006.10.10 **/
    memcpy(hv_fzf.br_no, QS_BR_NO , sizeof(hv_fzf.br_no)-1);
    set_zd_data("0030", hv_fzf.br_no);
  
    g_reply_int = iGetReceTel(hv_fzf.br_no);    /*  ��Ա�� ==> 0070 */
    if ( g_reply_int )
    {
      sprintf(acErrMsg,"�õ���Ա�Ŵ���!");
      WRITEMSG
      return -1;
    }
    get_zd_data("0070",tel);
    vtcp_log("%s,%d, tel======[%.8s]",__FILE__,__LINE__,tel );  
    memcpy(hv_fzf.tel,tel,sizeof(hv_fzf.tel)-1);    
      
      vtcp_log("%s,%d, ======hv_fzf.br_no=[%s] ",__FILE__,__LINE__,hv_fzf.br_no);    
    /*   �ĳ�ȫ��ͳһ��֧���������  2006-11-8 15:34   */
    long iOrderno = iLvGetFzfOrderno();
    vtcp_log("%s,%d, ======iOrderno=[%ld] ",__FILE__,__LINE__,iOrderno);      
    apitoa(iOrderno, sizeof(hv_fzf.orderno)-1, hv_fzf.orderno);
    vtcp_log("%s,%d, ======hv_fzf.orderno=[%s] ",__FILE__,__LINE__,hv_fzf.orderno);    
    char cTime[9];
    memset(cTime,0,sizeof(cTime));    
    get_fd_data("0060", cTime);
    hv_fzf.tx_time = apatoi(cTime, sizeof(cTime)-1);
    vtcp_log("%s,%d, ======hv_fzf.tx_time=[%ld] ",__FILE__,__LINE__,hv_fzf.tx_time);
    /**End:   Added by Chen Ming   2006.10.10 **/    
  
   /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
   if(strlen(hv_fzf.sendco) < 4)
   {
        iGetSendco(hv_fzf.sendco, hv_fzf.pay_qs_no);
   }
   if(strlen(hv_fzf.receco) < 4)
   {
        iGetSendco(hv_fzf.receco, hv_fzf.cash_qs_no);
   }
   /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
  
    g_reply_int = Hv_fzf_Ins(hv_fzf,g_pub_tx.reply);
    if( g_reply_int)
    {
      vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
      sprintf( acErrMsg,"�����¼����%s,%d ERR=[%d]",__FL__,g_reply_int);
      set_zd_data(DC_GET_MSG,"����һ���¼�¼����");
      WRITEMSG
      return (-1);
    }
    
    
    iLvFzfToFzfhtr(&hv_fzf,&hv_fzfhtr);   /** ���µ� hv_fzf ->hv_fzfhtr�Ŀ���***/

     g_reply_int = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
     if(g_reply_int)
     {
             sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!g_reply_int==[%d]\n",g_reply_int);
             WRITEMSG
             strcpy(g_pub_tx.reply,"HV01");
             return (-1);
     }
     
     vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);    
    
  }
  else  if(memcmp(lv->CMTCode,"316",sizeof(lv->CMTCode))==0)
  {
    memcpy(hv_fzf.cmtno, lv->CMTCode, sizeof(hv_fzf.cmtno)-1);
    hv_fzf.tx_date= apatoi(pcLvGetDay(),8);
    lv_get_tag(lv,"30A",cWtdate);
    hv_fzf.wt_date = apatoi(cWtdate,8);
    lv_get_tag(lv,"C36",hv_fzf.pay_qs_no);
    lv_get_tag(lv,"C37",hv_fzf.cash_qs_no);
    lv_get_tag(lv,"011",hv_fzf.or_br_no);
    lv_get_tag(lv,"012",hv_fzf.ac_br_no);
    int iOrderno;
    /* iOrderno=iLvGetOrderno();  �ĳ�ȫ��ͳһ��֧���������  2006-11-8 15:34   */
    iOrderno=iLvGetFzfOrderno();
    if(iOrderno<=0){
      vtcp_log("[%s][%d]�õ�֧��������Ŵ���",__FILE__,__LINE__);
      sprintf( acErrMsg,"�õ�������Ŵ�[%s][%d]",__FL__);
      set_zd_data(DC_GET_MSG,"�õ�֧��������Ŵ���");
      WRITEMSG
      return (-1);
    }

    /* Begin Modified by ChenMing  
    apitoa(iOrderno,sizeof(hv_fzf.orderno),hv_fzf.orderno);    */
    apitoa(iOrderno,sizeof(hv_fzf.orderno)-1,hv_fzf.orderno);    
    /* End Modified by ChenMing  */    
    
    lv_get_tag(lv,"02D",hv_fzf.o_cmtno);
    lv_get_tag(lv,"CCO",hv_fzf.or_br_no);
    lv_get_tag(lv,"30I",cOwtdate);
    hv_fzf.o_wt_date = apatoi(cOwtdate,8);
    memset(cTmpstr , 0 , sizeof(cTmpstr));
    lv_get_tag(lv,"0BE",cTmpstr);
    lv_orderno_add0(cTmpstr,sizeof(hv_fzf.o_packid)-1,hv_fzf.o_packid);

    /***ҵ���ִ״̬00-�ɹ� 01-�ʺŲ����� ***/
    /***02-�������� 03-���� 90����������***/
    /**10���˻�������� 11���˻�״̬���� 20��ҵ���ѳ�����������ֹ����**/
    lv_get_tag(lv,"CIA",hv_fzf.filler);/***ҵ���ִ״̬ ***/

    memcpy(hv_fzf.br_no, QS_BR_NO , sizeof(hv_fzf.br_no)-1);
    set_zd_data("0030", hv_fzf.br_no);
    
    hv_fzf.hv_fzf_sts[0]= STAT_LZRECV1;
    hv_fzf.lw_ind[0]='2';
    hv_fzf.otype[0]='2';
    Hv_fzf_Debug(&hv_fzf);
    
    
  g_reply_int = iGetReceTel(hv_fzf.br_no);    /*  ��Ա�� ==> 0070 */
  if ( g_reply_int )
  {
    sprintf(acErrMsg,"�õ���Ա�Ŵ���!");
    WRITEMSG
    return -1;
  }
  get_zd_data("0070",tel);
  vtcp_log("%s,%d, tel======[%.8s]",__FILE__,__LINE__,tel );  
  memcpy(hv_fzf.tel,tel,sizeof(hv_fzf.tel)-1);      

   /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
   if(strlen(hv_fzf.sendco) < 4)
   {
        iGetSendco(hv_fzf.sendco, hv_fzf.pay_qs_no);
   }
   if(strlen(hv_fzf.receco) < 4)
   {
        iGetSendco(hv_fzf.receco, hv_fzf.cash_qs_no);
   }
   /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/


    g_reply_int = Hv_fzf_Ins(hv_fzf,g_pub_tx.reply);
    if( g_reply_int)
    {
      vtcp_log("[%s][%d]g_reply_int==[%d]",__FILE__,__LINE__,g_reply_int);
      sprintf( acErrMsg,"�����¼����%s,%d ERR=[%d]",__FL__,g_reply_int);
      set_zd_data(DC_GET_MSG,"����һ���¼�¼����");
      WRITEMSG
      return (-1);
    }

    struct hv_fzfhtr_c hv_fzfhtr;
    memset(&hv_fzfhtr,0, sizeof(struct hv_fzfhtr_c));
    
    iLvFzfToFzfhtr(&hv_fzf,&hv_fzfhtr);   /** ���µ� hv_fzf ->hv_fzfhtr�Ŀ���***/

     g_reply_int = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
     if(g_reply_int)
     {
             sprintf(acErrMsg,"�������ݱ�hv_fzfhtr����!g_reply_int==[%d]\n",g_reply_int);
             WRITEMSG
             strcpy(g_pub_tx.reply,"HV01");
             return (-1);
     }
     
     vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr�ɹ�!\n",__FILE__,__LINE__);    
         
    
  }
  else /*Added by ChenMing 2006.09.26.*/ 
  {
      vtcp_log("[%s][%d]��������ˣ�CMTNO=[%s] �������ﴦ��! ",__FILE__,__LINE__,lv->CMTCode);  
  }
  return(0);
}

/* ����֧��ϵͳ�к���������ϵͳ�Ļ����ź���Ӧ��С����չ�Ա�� */
int pubf_lv_GetBr_noTel(char* or_br_no)
{
    char cOr_br_no[13];
    char cKinbr[BRNO_LEN+1];
    char cTel[7];
    int iRc=0;
    memset(cKinbr,0,sizeof(cKinbr));
    memset(cTel,0,sizeof(cTel));
    memset(cOr_br_no,0,sizeof(cOr_br_no));    
    memcpy(cOr_br_no,or_br_no, sizeof(cOr_br_no)-1);    
    
    if(GetBr_noByOr_br_no(cOr_br_no, cKinbr))
    {   
        vtcp_log("%s,%d, cKinbr======[%s]",__FILE__,__LINE__,cKinbr );
        vtcp_log("%s,%d, δ�ҵ�����Ϊ QS_BR_NO======[%s]",__FILE__,__LINE__,QS_BR_NO );  
        memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
    }
    vtcp_log("%s,%d,  cKinbr======[%s]",__FILE__,__LINE__,cKinbr );
    set_zd_data("0030",cKinbr);
    
    iRc = iGetReceTel(cKinbr);    /*  ��Ա�� ==> 0070 */
    if (iRc)
    {
      sprintf(acErrMsg,"�õ���Ա�Ŵ���!");
      WRITEMSG
      return -1;
    }
    get_zd_data("0070",cTel);
    vtcp_log("%s,%d, cTel======[%s]",__FILE__,__LINE__,cTel );    
  
    return 0;
}
